//
//  DataReporterManager.h
//  reporterPath
//
//  Created by luojilab on 2018/11/10.
//  Copyright © 2018年 luojilab. All rights reserved.
//

#import <Foundation/Foundation.h>

#ifdef DEBUG
#    define DebugLog(fmt, ...)   NSLog((@"[DEBUG]%s " fmt), __PRETTY_FUNCTION__, ##__VA_ARGS__);
#else
#    define DebugLog(...)
#endif

//如果需要建立多种类型的日志策略和格式
//需要创建多个instance
typedef NS_ENUM(NSUInteger, DataReporterType) {
    DataReporterTypeAction = 1,  //用户行为日志
    DataReporterTypeNetwork = 2, //网络级日志
};

@interface DataReporterManager : NSObject


/**
 start Report
 */
+ (void)startMonitorReport;



/**
 save ReportData

 @param data reportData
 */
+ (void)saveData:(NSData *)data;



/**
 ReaWaken - networkChange
 */
+ (void)reaWaken;


/**
 Stop - 结束上报
 */
+ (void)stopMonitorReport;


@end
