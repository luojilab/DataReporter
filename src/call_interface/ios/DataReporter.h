#import <Foundation/Foundation.h>

#define DATA_REPORTER_EXPORT __attribute__((visibility("default")))

DATA_REPORTER_EXPORT @interface DataReporter : NSObject
    
+ (void *)MakeReporter:(NSString *)uuid
             cachePath:(NSString *)cachePath
           uploadBlock:(void(^)(int64_t key,
                                NSArray *dataArray))uploadBlock;
    
+ (void)SetReportCount:(void *)nativeReporter
                 count:(NSInteger)count;
    
+ (void)SetFileMaxSize:(void *)nativeReporter
           fileMaxSize:(NSUInteger)fileMaxSize;
		   
+ (void)SetExpiredTime:(void *)nativeReporter
           expiredTime:(NSUInteger)expiredTime;
		   
+ (void)SetReportingInterval:(void *)nativeReporter
           reportingInterval:(NSUInteger)reportingInterval;
		   
+ (void)Start:(void *)nativeReporter;

+ (void)ReaWaken:(void *)nativeReporter;

+ (void)Push:(void *)nativeReporter
        data:(NSString *)data;

+ (void)UploadSucess:(void *)nativeReporter
                 key:(int64_t)key;
    
+ (void)UploadFailed:(void *)nativeReporter
                 key:(int64_t)key;
    
+ (void)ReleaseReporter:(void *)nativeReporter;
    
@end

