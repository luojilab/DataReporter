//
//  DataReporterManager.m
//  reporterPath
//
//  Created by luojilab on 2018/11/10.
//  Copyright © 2018年 luojilab. All rights reserved.
//

#import "DataReporterManager.h"
#import "DataReporter.h"


static NSString *kReporterIndentify = @"kReporterIndentify";
static NSUInteger kMaxFileSize = 100 * 1024;

//static instance
static void *reporterInstanse;

@interface DataReporterManager()

@property (nonatomic,copy) NSString *reporterPath;

@end

@implementation DataReporterManager

#pragma mark - life cycle

- (void)dealloc{
    [DataReporterManager stopMonitorReport];
}

+ (instancetype)sharedInstance {
    static id singleton = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        singleton = [[super allocWithZone:nil] initInstance];
    });
    return singleton;
}

- (instancetype)initInstance {
    self = [super init];
    if (self) {
      
    }
    return self;
}


#pragma mark - private Methods


/**
 初始化注册登录通知
 */
- (void)addNotification {
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
      
    });
}

/**
上报数据到服务器
*/
- (void)uploadData:(int64_t)key dataArrays:(NSArray *)dataArrays {
    
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
        bool successed = false;
        
        #warning TODO report to server-side
        
        if (random() % 3 == 1) {
            successed = true;
        } else {
            successed = false;
        }
        
        if (successed) {
            [self uploadSuccessed:key];
        } else {
            [self uploadFailed:key];
        }
    });
}

- (void)uploadSuccessed:(int64_t)key {
    //重要，通知DataReporter，report success，每次回调必须执行成功或者失败
    dispatch_async(dispatch_get_main_queue(), ^{
        [DataReporter UploadSucess:reporterInstanse key:key];
        DebugLog(@"UploadSucess -> should not upload again = %lld",key);
    });
}

- (void)uploadFailed:(int64_t)key {
    //重要，通知DataReporter，report Failed，每次回调必须执行成功或者失败
    //失败后，间隔一段时间会重新发送，上层不必多余处理
    dispatch_async(dispatch_get_main_queue(), ^{
        [DataReporter UploadFailed:reporterInstanse key:key];
        DebugLog(@"UploadFailed -> should upload again = %lld",key);
    });
}

/**
 初始化实例
 */
- (void)initReporterInstance{
    
    [DataReporterManager stopMonitorReport];

    reporterInstanse = [DataReporter MakeReporter:kReporterIndentify cachePath:self.reporterPath encryptKey:@"" uploadBlock:^(int64_t key, NSArray *dataArrays) {
        if (dataArrays== nil || [dataArrays count] == 0 || reporterInstanse == nil){
            return;
        }
       
        dispatch_async(dispatch_get_main_queue(), ^{
            [self uploadData:key dataArrays:dataArrays];
        });
    }];
    //set report max count  设置每次上报最大的数据量 10表示，一次最多10条报一次
    [DataReporter SetReportCount:reporterInstanse count:10];
	
	//set report ExpiredTime 0表示永久有效 所有数据上报，10*24*60*60 表示10天内有效，10天外数据不上报
    [DataReporter SetExpiredTime:reporterInstanse expiredTime:0];
	
	//set report reporterInstanse 上报间隔 单位i秒  10 表示每隔10秒上报一次，0表示有数据立即上报
    [DataReporter SetReportingInterval:reporterInstanse reportingInterval:0];
	
    //set save file size 设置缓存文件大小， 大小一定要比单条push进来的数据大
    [DataReporter SetFileMaxSize:reporterInstanse fileMaxSize:kMaxFileSize];
}
                               


/**
 开始上报任务
 */
- (void)startReporter{
    
    if (reporterInstanse) {
        [DataReporter Start:reporterInstanse];
    }
    
}



#pragma mark - public Methods


/**
 start Report
 */
+ (void)startMonitorReport{
    [[self sharedInstance] initReporterInstance];
    [[self sharedInstance] startReporter];
}


/**
 save ReportData
 
 @param data reportData
 */
+ (void)pushData:(NSData *)data{

    if ([data length] == 0){
        return;
    }
    if (!reporterInstanse) {
        return;
    }
    [DataReporter Push:reporterInstanse byteArray:data];
}


/**
 ReaWaken - networkChange
 */
+ (void)reaWaken{
    if (!reporterInstanse) {
        return;
    }
    [DataReporter ReaWaken:reporterInstanse];
}

/**
 Stop - 结束上报
 */
+ (void)stopMonitorReport{
    if (reporterInstanse == nil) {
        return;
    }
    [DataReporter ReleaseReporter:reporterInstanse];
    reporterInstanse = NULL;

}


#pragma mark - getters and setters


- (NSString *)reporterPath{
    if (_reporterPath == nil) {
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *documentsDirectory = [paths objectAtIndex:0];
        NSString *repoterDir = [documentsDirectory stringByAppendingPathComponent:@"DataRepoter"];
        BOOL isDir = NO;
        NSFileManager *fileManager = [NSFileManager defaultManager];
        BOOL existed = [fileManager fileExistsAtPath:repoterDir isDirectory:&isDir];
        if (!(isDir == YES && existed == YES)){
            [fileManager createDirectoryAtPath:repoterDir withIntermediateDirectories:YES attributes:nil error:nil];
        }
        _reporterPath = repoterDir;
    }
    return _reporterPath;
}

@end
