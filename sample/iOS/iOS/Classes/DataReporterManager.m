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
 初始化实例
 */
- (void)initReporterInstance{
    
    [DataReporterManager stopMonitorReport];

    reporterInstanse = [DataReporter MakeReporter:kReporterIndentify cachePath:self.reporterPath uploadBlock:^(int64_t key, NSArray *dataArray) {
        if (dataArray == nil || [dataArray count] == 0 || reporterInstanse == nil){
            return;
        }
       
#warning TODO report to server-side
        DebugLog(@"need report to server-side = %ld",(long)[dataArray count]);
        if (random() % 3 == 1) {
            //重要，通知DataReporter，report success，每次回调必须执行成功或者失败
            [DataReporter UploadSucess:reporterInstanse key:key];
            DebugLog(@"UploadSucess -> should not upload again = %lld",key);
        }else{
            //重要，通知DataReporter，report Failed，每次回调必须执行成功或者失败
            //失败后，间隔一段时间会重新发送，上层不必多余处理
            [DataReporter UploadFailed:reporterInstanse key:key];
            DebugLog(@"UploadFailed -> should upload again = %lld",key);
        }
    }];
    //set report max count
    [DataReporter SetReportCount:reporterInstanse count:10];
    //set save file size
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
+ (void)saveData:(NSString *)data{

    if ([data length] == 0){
        return;
    }
    if (!reporterInstanse) {
        return;
    }
    [DataReporter Push:reporterInstanse data:data];
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
