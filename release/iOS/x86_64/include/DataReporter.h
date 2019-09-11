#import <Foundation/Foundation.h>

#define DATA_REPORTER_EXPORT __attribute__((visibility("default")))

DATA_REPORTER_EXPORT @interface DataReporter : NSObject

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
		   
+ (void)Start:(void *)nativeReporter;

+ (void)ReaWaken:(void *)nativeReporter;

+ (void)Push:(void *)nativeReporter
   byteArray:(NSData *)byteArray;

+ (void)UploadSucess:(void *)nativeReporter
                 key:(int64_t)key;
    
+ (void)UploadFailed:(void *)nativeReporter
                 key:(int64_t)key;
    
+ (void)ReleaseReporter:(void *)nativeReporter;
    
@end

