//
// Created by bingjian on 2018/10/29.
//

#ifndef DATA_REPORTER_REPORTER_H
#define DATA_REPORTER_REPORTER_H

#include <string>
#include <functional>
#include <atomic>
#include <map>
#include <set>
#include "MmapedFile.h"
#include "HandlerThread.h"
#include "MemoryStream.h"
#include "DataProvider.h"

namespace future {
    class Reporter {
    public:
        Reporter(const std::string &uuid, const std::string &cachePath, const std::string &encryptKey,
                 std::function<void(int64_t key,
                                    std::list<std::shared_ptr<CacheItem> > &data)> netImpl);

        static void Destroy(Reporter *reporter);

        static void *GetValidMem(Buffer &buffer);

        void SetUploadItemCount(std::size_t itemSize);

        void SetFileMaxSize(std::size_t fileMaxSize);

        void SetExpiredTime(std::int64_t expiredTime);

        void SetReportingInterval(std::int64_t reportingInterval);

        void Push(const std::vector<unsigned char> &data);

        void UoloadSuccess(int64_t key);

        void UploadFailed(int64_t key);

        void WrtiteToFile();

        void Report();

        void Start();

        void ReaWaken();

    private:

        ~Reporter();

        void CheckDataBuf();

        void CheckWriteBuf();

        std::int64_t DumpDataBuf(void *addr, long maxSize);

        bool IsAsyncWriteFile();

        bool IsSyncWriteFile(const std::size_t dataLen);

        std::string MakeFileName(const std::string &path);

        void DelayReport();

        void ClearDelayUploadTasks();

        void ClearDelayReportTasks();


    public:
        static const std::string DATA_SUFFIX;

    private:
        static WTF::HandlerThread *s_HandlerThread;
        static std::atomic_int s_ReporterCount;

        std::mutex m_Mut;

        const std::string m_UUid;
        const std::string m_CachePath;
        std::atomic_int m_ItemSize;
        std::atomic_uint m_MaxFileSize;
        std::atomic_int_least64_t m_ExpiredTime;
        std::atomic_int_least64_t m_ReportingInterval;
        std::atomic_bool m_IsStart;
        std::shared_ptr<Buffer> m_DataBuf;
        std::shared_ptr<MemoryStream> m_MemoryStream;
        std::shared_ptr<Buffer> m_WriteFileBuf;
        std::shared_ptr<Buffer> m_UploadBuf;

        std::shared_ptr<MmapedFile> m_DataMmapFile;
        std::shared_ptr<MmapedFile> m_WriteFileMmapFile;
        std::shared_ptr<MmapedFile> m_UploadMmapFile;

        std::shared_ptr<DataProvider> m_DataProvider;
        std::map<int64_t, std::shared_ptr<std::list<std::shared_ptr<CacheItem> > > > m_Reporting;
        std::set<std::shared_ptr<WTF::TimeTask> > m_DelayUploadTasks;
        std::set<std::shared_ptr<WTF::TimeTask> > m_DelayReportTasks;

        std::function<void(int64_t key, std::list<std::shared_ptr<CacheItem> > &data)> m_UploadImpl;
        std::function<void(void)> m_ReportFun;
        std::function<void(void)> m_WriteFileFun;
        std::atomic_int m_RetryStep;
        std::string m_EncryptKey;
        std::function<void *(void *, std::size_t, std::size_t &)> m_EncryptFun;
        std::function<void *(void *, std::size_t, std::size_t &)> m_DecryptFun;

    };
}


#endif //DATA_REPORTER_REPORTER_H
