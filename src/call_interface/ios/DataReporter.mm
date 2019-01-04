#include <string>

#include "Reporter.h"
#include "CacheItem.h"

#import "DataReporter.h"

@implementation DataReporter

+ (void *)MakeReporter:(NSString *)uuid
             cachePath:(NSString *)cachePath
           uploadBlock:(void(^)(int64_t key,
                                NSArray *dataArray))uploadBlock
{
    if (uuid == nil || cachePath == nil) {
        return nullptr;
    }
    
    std::string uuidCStr = [uuid UTF8String];
    std::string cachePathCstr = [cachePath UTF8String];
    future::Reporter *reporter = new future::Reporter(uuidCStr,
                                 cachePathCstr,
                                 [uploadBlock](int64_t key,
                                               std::list<std::shared_ptr<future::CacheItem> > &data) {
                                     if (uploadBlock && !data.empty()) {
                                         NSMutableArray *resultData = [[NSMutableArray alloc] init];
                                         std::list<std::shared_ptr<future::CacheItem> >::iterator iter = data.begin();
                                         for (; iter != data.end(); iter++) {
                                             std::string dataCstr;
                                             dataCstr.append((const char *) (*iter)->pbEncodeItem.data.GetBegin(),
                                                             (*iter)->pbEncodeItem.data.Length());
                                             [resultData addObject:[NSString stringWithUTF8String:dataCstr.c_str()]];
                                         }
                                         
                                         uploadBlock(key, resultData);
                                     }
                                 });
    return reporter;
}

+ (void)SetReportCount:(void *)nativeReporter
                 count:(NSInteger)count
{
    future::Reporter *reporter = reinterpret_cast<future::Reporter *>(nativeReporter);
    if (reporter == NULL) {
        return;
    }
    reporter->SetUploadItemSize((int) count);
}

+ (void)SetFileMaxSize:(void *)nativeReporter
           fileMaxSize:(NSUInteger)fileMaxSize {
    
    future::Reporter *reporter = reinterpret_cast<future::Reporter *>(nativeReporter);
    if (reporter == NULL) {
        return;
    }
    reporter->SetFileMaxSize(fileMaxSize);
}

+ (void)SetExpiredTime:(void *)nativeReporter
           expiredTime:(int64_t)expiredTime {
    
    future::Reporter *reporter = reinterpret_cast<future::Reporter *>(nativeReporter);
    if (reporter == NULL) {
        return;
    }
    reporter->SetExpiredTime(expiredTime);
}

+ (void)SetReportingInterval:(void *)nativeReporter
           reportingInterval:(int64_t)reportingInterval {
    
    future::Reporter *reporter = reinterpret_cast<future::Reporter *>(nativeReporter);
    if (reporter == NULL) {
        return;
    }
    reporter->SetReportingInterval(reportingInterval);
}
    
+ (void)Start:(void *)nativeReporter {
    future::Reporter *reporter = reinterpret_cast<future::Reporter *>(nativeReporter);
    if (reporter == NULL) {
        return;
    }
    reporter->Start();
}

+ (void)ReaWaken:(void *)nativeReporter {
    future::Reporter *reporter = reinterpret_cast<future::Reporter *>(nativeReporter);
    if (reporter == NULL) {
        return;
    }
    reporter->ReaWaken();
}

+ (void)Push:(void *)nativeReporter
        data:(NSString *)data
{
    future::Reporter *reporter = reinterpret_cast<future::Reporter *>(nativeReporter);
    if (reporter == NULL || data == nil) {
        return;
    }
    std::string dataCstr = [data UTF8String];
    reporter->Push(dataCstr);
}

+ (void)UploadSucess:(void *)nativeReporter
                 key:(int64_t)key
{
    future::Reporter *reporter = reinterpret_cast<future::Reporter *>(nativeReporter);
    if (reporter == NULL) {
        return;
    }
    reporter->UoloadSuccess(key);
}

+ (void)UploadFailed:(void *)nativeReporter
                 key:(int64_t)key
{
    future::Reporter *reporter = reinterpret_cast<future::Reporter *>(nativeReporter);
    if (reporter == NULL) {
        return;
    }
    reporter->UploadFailed(key);
}

+ (void)ReleaseReporter:(void *)nativeReporter
{
    future::Reporter *reporter = reinterpret_cast<future::Reporter *>(nativeReporter);
    if (reporter == NULL) {
        return;
    }
    
    future::Reporter::Destroy(reporter);
}

@end



