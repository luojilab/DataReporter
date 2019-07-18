//
// Created by bingjian on 2018/10/29.
//

#include "File.h"
#include "RawOutput.h"
#include "Reporter.h"
#include "MiniPBCoder.h"
#include "TimeUtil.h"
#include "StringUtil.h"
#include "IoUtil.h"

namespace future {

    std::atomic_int Reporter::s_ReporterCount(0);
    WTF::HandlerThread *Reporter::s_HandlerThread = NULL;

    const std::string Reporter::DATA_SUFFIX = ".data";
    const int RETRY_STEP = 1000 * 5;

    Reporter::Reporter(const std::string &uuid, const std::string &cachePath,
                       std::function<void(int64_t key,
                                          std::list<std::shared_ptr<CacheItem> > &data)> netImpl)
            : m_CachePath(cachePath), m_MaxFileSize(1024 * 10), m_ItemSize(5), m_UUid(uuid),
              m_RetryStep(RETRY_STEP),
              m_ExpiredTime(0),//in second
              m_ReportingInterval(0),//in millisecond
              m_DataProvider(NULL),
              m_DataBuf(NULL),
              m_WriteFileBuf(NULL),
              m_UploadBuf(NULL),
              m_MemoryStream(NULL),
              m_IsStart(false),
              m_UploadImpl(netImpl) {
        std::lock_guard<std::mutex> lk(m_Mut);
        m_ThreadId = std::this_thread::get_id();

        if (!File::IsFileExist(cachePath)) {
            File::MkPath(cachePath);
        }

        if (s_ReporterCount == 0) {
            Debug("Reporter addr:%p new WTF::HandlerThread()", this);
            s_HandlerThread = new WTF::HandlerThread();
        }
        s_ReporterCount++;
        m_ReportFun = std::bind(&Reporter::Report, this);
        m_WriteFileFun = std::bind(&Reporter::WrtiteToFile, this);
    }

    void Reporter::Destroy(Reporter *reporter) {
        if (reporter->GetThreadId() != std::this_thread::get_id()) {
            return;
        }
        Debug("Destroy addr:%p", reporter);
        s_HandlerThread->postMsg([reporter]() {
            delete reporter;
        });
    }

    Reporter::~Reporter() {
        std::lock_guard<std::mutex> lk(m_Mut);
        Debug("~Reporter addr:%p", this);

        if (m_DataMmapFile != NULL) {
            if (m_DataMmapFile->IsOpened()) {
                m_DataMmapFile->Close();
                m_DataMmapFile = NULL;
            }
        }

        if (m_WriteFileMmapFile != NULL) {
            if (m_WriteFileMmapFile->IsOpened()) {
                m_WriteFileMmapFile->Close();
                m_WriteFileMmapFile = NULL;
            }
        }

        if (m_UploadMmapFile != NULL) {
            if (m_UploadMmapFile->IsOpened()) {
                m_UploadMmapFile->Close();
                m_UploadMmapFile = NULL;
            }
        }

        ClearDelayUploadTasks();
        ClearDelayReportTasks();

        s_ReporterCount--;
        if (s_ReporterCount == 0) {
            s_HandlerThread->postMsg([]() {
                s_HandlerThread->stop();
            });
        }
    }

    void Reporter::SetUploadItemSize(std::size_t itemSize) {
        if (m_ThreadId != std::this_thread::get_id()) {
            return;
        }
        m_ItemSize = itemSize;
    }

    void Reporter::SetFileMaxSize(std::size_t fileMaxSize) {
        if (m_ThreadId != std::this_thread::get_id()) {
            return;
        }
        m_MaxFileSize = fileMaxSize;
    }

    void Reporter::SetExpiredTime(std::int64_t expiredTime) {
        if (m_ThreadId != std::this_thread::get_id()) {
            return;
        }
        m_ExpiredTime = expiredTime;
    }

    void Reporter::SetReportingInterval(std::int64_t reportingInterval) {
        if (m_ThreadId != std::this_thread::get_id()) {
            return;
        }
        m_ReportingInterval = reportingInterval;
    }

    void Reporter::Push(const std::string &data) {
        std::lock_guard<std::mutex> lk(m_Mut);
        Debug("Push addr:%p", this);
        if (m_ThreadId != std::this_thread::get_id()) {
            return;
        }
        if (IsSyncWriteFile(data)) {
            WrtiteToFile();
        }
        std::int64_t now = TimeUtil::GetSystemClockSecondsTime();
        std::string nowStr = Int64ToStr(now);
        m_MemoryStream->Write(data, nowStr);

        s_HandlerThread->postMsg([this]() {
            if (m_Reporting.empty()) {
                DelayReport();
            }
        });

        if (IsAsyncWriteFile()) {
            s_HandlerThread->postMsg(m_WriteFileFun);
        }
    }

    void Reporter::UoloadSuccess(int64_t key) {
        Debug("UoloadSuccess addr:%p", this);
        if (m_ThreadId != std::this_thread::get_id()) {
            return;
        }
        m_RetryStep = RETRY_STEP;
        s_HandlerThread->postMsg([this, key]() {
            std::map<int64_t, std::list<std::shared_ptr<CacheItem> > >::iterator iter = m_Reporting.find(
                    key);
            if (iter != m_Reporting.end()) {
                for (std::list<std::shared_ptr<CacheItem> >::iterator cacheIter = iter->second.begin();
                     cacheIter != iter->second.end(); cacheIter++) {
                    m_DataProvider->ClearItem(*(*cacheIter));
                }
            }
            m_Reporting.erase(key);
            DelayReport();
        });
    }

    void Reporter::UploadFailed(int64_t key) {
        Debug("UploadFailed addr:%p", this);
        if (m_ThreadId != std::this_thread::get_id()) {
            return;
        }

        s_HandlerThread->postMsg([this, key]() {
            m_RetryStep += RETRY_STEP;
            std::shared_ptr<WTF::TimeTask> delayTask(new WTF::TimeTask(m_RetryStep, 0, NULL));
            std::weak_ptr<WTF::TimeTask> weakDelayTask = delayTask;
            delayTask->setFun([this, key, weakDelayTask]() {
                std::map<int64_t, std::list<std::shared_ptr<CacheItem> > >::iterator iter = m_Reporting.find(
                        key);
                if (m_UploadImpl != NULL && iter != m_Reporting.end()) {
                    m_UploadImpl(key, m_Reporting[key]);
                }
                m_DelayUploadTasks.erase(weakDelayTask.lock());
            });

            m_DelayUploadTasks[delayTask] = 0;
            s_HandlerThread->postPeriodTask(*delayTask);
        });
    }

    void Reporter::Report() {
        Debug("Report addr:%p", this);
        if (!m_Reporting.empty()) {
            return;
        }

        std::list<std::shared_ptr<CacheItem> > data = m_DataProvider->ReadData(m_ItemSize,
                                                                               m_ExpiredTime);
        if (data.empty()) {
            return;
        }

        if (m_UploadImpl != NULL) {
            int64_t now = TimeUtil::GetSteadyClockNanoTime();
            std::map<int64_t, std::list<std::shared_ptr<CacheItem> > >::iterator reportingIter = m_Reporting.find(
                    now);
            while (reportingIter != m_Reporting.end()) {
                now = TimeUtil::GetSteadyClockNanoTime();
                reportingIter = m_Reporting.find(now);
            }

            m_Reporting[now] = std::move(data);
            m_UploadImpl(now, m_Reporting[now]);
        }
    }

    void Reporter::Start() {
        std::lock_guard<std::mutex> lk(m_Mut);
        if (m_ThreadId != std::this_thread::get_id()) {
            return;
        }
        if (m_IsStart) {
            return;
        }
        m_IsStart = true;

        //缓存是设置文件最大大小的2倍，防止文件写入过慢，缓存过满的情况
        m_DataMmapFile = std::shared_ptr<MmapedFile>(
                new MmapedFile(m_CachePath + "/" + m_UUid + "_data.buf", m_MaxFileSize * 2));
        if (m_DataMmapFile->Open()) {
            m_DataBuf = std::shared_ptr<Buffer>(
                    new Buffer(m_DataMmapFile->GetMemBegin(), m_DataMmapFile->GetMaxSize(),
                               BufferNoCopy));
        } else {
            m_DataBuf = std::shared_ptr<Buffer>(
                    new Buffer(m_DataMmapFile->GetMaxSize()));
            File::RemoveFile(m_DataMmapFile->GetPath());
            m_DataMmapFile = NULL;
        }

        m_WriteFileMmapFile = std::shared_ptr<MmapedFile>(
                new MmapedFile(m_CachePath + "/" + m_UUid + "_writefile.buf", m_MaxFileSize * 2));

        m_UploadMmapFile = std::shared_ptr<MmapedFile>(
                new MmapedFile(m_CachePath + "/" + m_UUid + "_upload.buf", m_MaxFileSize * 2));

        if (m_UploadMmapFile->Open()) {
            m_UploadBuf = std::shared_ptr<Buffer>(
                    new Buffer(m_UploadMmapFile->GetMemBegin(), m_UploadMmapFile->GetMaxSize(),
                               BufferNoCopy));
        } else {
            m_UploadBuf = std::shared_ptr<Buffer>(
                    new Buffer(m_UploadMmapFile->GetMaxSize()));
            File::RemoveFile(m_UploadMmapFile->GetPath());
            m_UploadMmapFile = NULL;
        }

        if (m_WriteFileMmapFile->Open()) {
            m_WriteFileBuf = std::shared_ptr<Buffer>(
                    new Buffer(m_WriteFileMmapFile->GetMemBegin(),
                               m_WriteFileMmapFile->GetMaxSize(),
                               BufferNoCopy));
        } else {
            m_WriteFileBuf = std::shared_ptr<Buffer>(
                    new Buffer(m_WriteFileMmapFile->GetMaxSize()));
            File::RemoveFile(m_WriteFileMmapFile->GetPath());
            m_WriteFileMmapFile = NULL;
        }

        m_MemoryStream = std::shared_ptr<MemoryStream>(new MemoryStream(m_DataBuf));

        std::function<std::int64_t(void *, long)> dumpDataFun = std::bind(&Reporter::DumpDataBuf,
                                                                          this,
                                                                          std::placeholders::_1,
                                                                          std::placeholders::_2);
        m_DataProvider = std::shared_ptr<DataProvider>(
                new DataProvider(m_CachePath, m_UploadBuf, dumpDataFun));

        CheckDataBuf();
        CheckWriteBuf();
        s_HandlerThread->postMsg(m_ReportFun);
        s_HandlerThread->start();
    }

    void Reporter::ReaWaken() {
        std::lock_guard<std::mutex> lk(m_Mut);
        if (m_ThreadId != std::this_thread::get_id()) {
            return;
        }

        s_HandlerThread->postMsg([this]() {
            std::lock_guard<std::mutex> lk(m_Mut);
            if (m_DelayUploadTasks.empty()) {
                return;
            }
            ClearDelayUploadTasks();
            if (m_Reporting.empty()) {
                return;
            }
            m_UploadImpl(m_Reporting.begin()->first, m_Reporting.begin()->second);
        });
    }

    std::thread::id &Reporter::GetThreadId() {
        return m_ThreadId;
    }

    void Reporter::WrtiteToFile() {
        if (!IsAsyncWriteFile()) {
            return;
        }
        std::string fileName = MakeFileName(m_CachePath);
        m_MemoryStream->MoveToFile(fileName, m_WriteFileBuf->GetBegin());
        m_WriteFileBuf->Clear();
    }

    void Reporter::CheckDataBuf() {
        void *pos = GetValidMem(*m_DataBuf);
        if (pos == NULL || m_DataBuf->GetBegin() == pos) {
            return;
        }

        long offset = (unsigned char *) pos - (unsigned char *) m_DataBuf->GetBegin();
        m_MemoryStream->Clear(offset);
        m_MemoryStream->Seek(offset);
    }

    void Reporter::CheckWriteBuf() {
        void *pos = GetValidMem(*m_WriteFileBuf);
        if (pos != NULL && pos != m_WriteFileBuf->GetBegin()) {
            std::string fileName = MakeFileName(m_CachePath);
            IoUtil::WriteFile(fileName, m_WriteFileBuf->GetBegin(),
                              (unsigned char *) pos -
                              (unsigned char *) m_WriteFileBuf->GetBegin());
        }
    }

    void *Reporter::GetValidMem(Buffer &buffer) {
        void *begin = buffer.GetBegin();
        if (begin == NULL) {
            return NULL;
        }

        std::size_t bufferSize = buffer.Length();
        std::size_t itemLen = 0;

        for (int i = 0; i < bufferSize; i++) {
            unsigned char *pos = (unsigned char *) begin + itemLen;
            Buffer decodeBuffer(pos, bufferSize - itemLen, BufferNoCopy);
            PBEncodeItem item = MiniPBCoder::DecodeItem(decodeBuffer);
            if (!MiniPBCoder::VerifyItem(item)) {
                return pos;
            }
            itemLen += MiniPBCoder::CalculatedSize(item);
        }
        return buffer.GetEnd();
    }

    std::int64_t Reporter::DumpDataBuf(void *addr, long maxSize) {
        std::int64_t ret = m_MemoryStream->MoveToMem(addr, maxSize);
        return ret;
    }

    bool Reporter::IsAsyncWriteFile() {
        return m_MemoryStream->GetOffset() > m_MemoryStream->Size() / 2;
    }

    bool Reporter::IsSyncWriteFile(const std::string &data) {
        return (m_MemoryStream->GetOffset() + MiniPBCoder::CalculatedSize(data) >=
                m_MemoryStream->Size());
    }

    std::string Reporter::MakeFileName(const std::string &path) {
        std::int64_t nanoTime = TimeUtil::GetSystemClockNanoTime();
        std::string ret = path + "/" + Int64ToStr(nanoTime) + DATA_SUFFIX;
        return ret;
    }

    void Reporter::DelayReport() {
        if (!m_DelayReportTasks.empty()) {
            return;
        }
        std::shared_ptr<WTF::TimeTask> delayTask(
                new WTF::TimeTask(m_ReportingInterval, 0, NULL));
        
        std::weak_ptr<WTF::TimeTask> weakDelayTask = delayTask;
        delayTask->setFun([this, weakDelayTask]() {
            if (m_ReportFun != NULL) {
                m_ReportFun();
            }
            m_DelayReportTasks.erase(weakDelayTask.lock());
        });

        m_DelayReportTasks[delayTask] = 0;
        s_HandlerThread->postPeriodTask(*delayTask);
    }

    void Reporter::ClearDelayUploadTasks() {
        for (std::map<std::shared_ptr<WTF::TimeTask>, int>::iterator iter = m_DelayUploadTasks.begin();
             iter != m_DelayUploadTasks.end(); iter++) {
            s_HandlerThread->cancelPeriodTask(*iter->first);
        }
        m_DelayUploadTasks.clear();
    }

    void Reporter::ClearDelayReportTasks() {
        for (std::map<std::shared_ptr<WTF::TimeTask>, int>::iterator iter = m_DelayReportTasks.begin();
             iter != m_DelayReportTasks.end(); iter++) {
            s_HandlerThread->cancelPeriodTask(*iter->first);
        }
        m_DelayReportTasks.clear();
    }

}
