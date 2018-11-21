#import <Foundation/Foundation.h>

#define DATA_REPORTER_EXPORT __attribute__((visibility("default")))

DATA_REPORTER_EXPORT @interface DataReporter : NSObject
    
+ (void *)MakeReporter:(NSString *)uuid
             cachePath:(NSString *)cachePath
           uploadBlock:(void(^)(int64_t key,
                                NSArray *dataArray))uploadBlock;

+ (void)Push:(void *)nativeReporter
        data:(NSString *)data;
    
+ (void)SetReportCount:(void *)nativeReporter
                 count:(NSInteger)count;
    
+ (void)UploadSucess:(void *)nativeReporter
                 key:(NSInteger)key;
    
+ (void)UploadFailed:(void *)nativeReporter
                 key:(NSInteger)key;
    
+ (void)Start:(void *)nativeReporter;
    
+ (void)SetUploadItemSize:(void *)nativeReporter
                 itemSize:(NSUInteger)itemSize;
    
+ (void)SetFileMaxSize:(void *)nativeReporter
           fileMaxSize:(NSUInteger)fileMaxSize;

+ (void)ReaWaken:(void *)nativeReporter;
    
+ (void)ReleaseReporter:(void *)nativeReporter;
    
@end

