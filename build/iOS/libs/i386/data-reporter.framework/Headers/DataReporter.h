#import <Foundation/Foundation.h>

#define DATA_REPORTER_EXPORT __attribute__((visibility("default")))

DATA_REPORTER_EXPORT @interface DataReporter : NSObject

+ (void *)MakeReporter:(NSString *)uuid
             cachePath:(NSString *)cachePath
           uploadBlock:(void(^)(int64_t key,
                                NSArray *byteArray))uploadBlock __deprecated_msg("已废弃，请勿使用。");

/*
 byteArrays包含NSData数组。
 */
+ (void *)MakeReporter:(NSString *)uuid
             cachePath:(NSString *)cachePath
            encryptKey:(NSString *)encryptKey
           uploadBlock:(void(^)(int64_t key,
                                NSArray *byteArrays))uploadBlock;
    
+ (void)SetReportCount:(void *)nativeReporter
                 count:(NSInteger)count;
    
+ (void)SetFileMaxSize:(void *)nativeReporter
           fileMaxSize:(NSUInteger)fileMaxSize;
		   
+ (void)SetExpiredTime:(void *)nativeReporter
           expiredTime:(int64_t)expiredTime;
		   
+ (void)SetReportingInterval:(void *)nativeReporter
           reportingInterval:(int64_t)reportingInterval;
		   
+ (void)SetRetryInterval:(void *)nativeReporter
           retryInterval:(int64_t)retryInterval;
		   
+ (void)Start:(void *)nativeReporter;

+ (void)ReaWaken:(void *)nativeReporter;

+ (void)Push:(void *)nativeReporter
   byteArray:(NSData *)byteArray;

+ (void)Push:(void *)nativeReporter
        data:(NSString *)data __deprecated_msg("已废弃，请勿使用。");

+ (void)UploadSucess:(void *)nativeReporter
                 key:(int64_t)key;
    
+ (void)UploadFailed:(void *)nativeReporter
                 key:(int64_t)key;
    
+ (void)ReleaseReporter:(void *)nativeReporter;
    
@end

