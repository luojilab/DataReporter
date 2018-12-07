//
// Created by bingjian on 2018/11/6.
//

#include <File.h>
#include "DataProvider.h"
#include "StringUtil.h"
#include "FileInputStream.h"
#include "MiniPBCoder.h"
#include "Reporter.h"
#include "NanoTime.h"


namespace future {

    bool DataProvider::IsExpired(std::int64_t date, std::int64_t expiredTime) {
        if (expiredTime == 0 || date == 0) {
            return false;
        }
        std::int64_t now = GetNanoTime();
        std::int64_t deadLine = date + expiredTime * 1000000;
        if (now > deadLine) {
            return true;
        }
        return false;
    }

    DataProvider::DataProvider(const std::string &fromPath, std::shared_ptr<Buffer> &fromMem,
                               std::function<std::int64_t(void *, int)> updateMem)
            : m_FromPath(fromPath), m_FromMem(fromMem), m_UpdateMem(updateMem),
              m_MemDataEndPos(NULL), m_MemOffset(NULL), m_IsUploadingMem(false),
              m_FileInputStream(NULL) {
        m_MemOffset = fromMem->GetBegin();
        m_MemDataEndPos = Reporter::GetValidMem(*fromMem);
    }

    DataProvider::~DataProvider() {
        m_MemOffset = NULL;
        m_MemDataEndPos = NULL;
        if (m_FileInputStream != NULL) {
            m_FileInputStream->Close();
            m_FileInputStream = NULL;
        }
    }

    std::list<std::shared_ptr<CacheItem> >
    DataProvider::ReadData(std::size_t count, std::int64_t expiredTime) {
        std::list<std::shared_ptr<CacheItem> > ret;
        std::list<std::shared_ptr<CacheItem> > memData = ReadFromMem(count, expiredTime);
        if (!memData.empty()) {
            m_IsUploadingMem = true;
            return memData;
        }

        long readCount = count - ret.size();
        std::list<std::shared_ptr<CacheItem> > fileData = ReadFromFile(readCount, expiredTime);
        if (fileData.size() == readCount) {
            ret.insert(ret.end(), fileData.begin(), fileData.end());
            return ret;
        }
        ret.insert(ret.end(), fileData.begin(), fileData.end());

        readCount = count - ret.size();
        if (m_UpdateMem != NULL && !m_IsUploadingMem) {
            std::int64_t updateCount = m_UpdateMem(m_FromMem->GetBegin(),
                                                   m_FromMem->Length());
            m_MemDataEndPos = (unsigned char *) m_FromMem->GetBegin() + updateCount;
            m_MemOffset = m_FromMem->GetBegin();

            if (m_MemDataEndPos != m_FromMem->GetBegin() &&
                m_MemDataEndPos != m_FromMem->GetEnd()) {
                std::list<std::shared_ptr<CacheItem> > memData = ReadFromMem(readCount,
                                                                             expiredTime);
                ret.insert(ret.end(), memData.begin(), memData.end());
            }
        }
        return ret;
    }

    std::list<std::shared_ptr<CacheItem> >
    DataProvider::ReadFromFile(std::size_t count, std::int64_t expiredTime) {
        std::list<std::shared_ptr<CacheItem> > ret;

        for (int i = 0; i < count; ) {
            if (m_Files.empty()) {
                m_Files = ListFiles();
                if (m_Files.empty()) {
                    return ret;
                }
            }

            if (m_FileInputStream == NULL) {
                const std::string &firstFile = m_Files.front();
                m_FileInputStream = std::shared_ptr<FileInputStream>(
                        new FileInputStream(m_FromPath + "/" + firstFile));
                m_UploadingFile[firstFile] = 0;
            }

            if (!m_FileInputStream->IsOpened()) {
                if (!m_FileInputStream->Open()) {
                    std::string filePath = m_FileInputStream->GetPath();
                    std::string fileNameWithoutExt = File::GetFileNameWithExt(filePath);
                    m_UploadingFile.erase(fileNameWithoutExt);
                    m_FileInputStream = NULL;
                    m_Files.pop_front();
                    continue;
                }
            }

            std::list<std::shared_ptr<CacheItem> > items = m_FileInputStream->ReadData(
                    count - ret.size(), expiredTime);
            if (items.empty()) {
                std::string filePath = m_FileInputStream->GetPath();
                if (m_FileInputStream->IsOpened()) {
                    m_FileInputStream->Close();
                }
                m_FileInputStream = NULL;
                File::RemoveFile(filePath);
                std::string fileNameWithoutExt = File::GetFileNameWithExt(filePath);
                m_UploadingFile.erase(fileNameWithoutExt);
            }

            i += items.size();
            ret.insert(ret.end(), items.begin(), items.end());
            if (ret.size() == count) {
                return ret;
            }

            if (m_FileInputStream != NULL) {
                if (m_FileInputStream->IsOpened()) {
                    m_FileInputStream->Close();
                }
                m_FileInputStream = NULL;
            }

            m_Files.pop_front();
        }

        return ret;
    }

    std::list<std::shared_ptr<CacheItem> >
    DataProvider::ReadFromMem(std::size_t count, std::int64_t expiredTime) {
        std::list<std::shared_ptr<CacheItem> > ret;
        if (m_MemOffset == m_MemDataEndPos) {
            return ret;
        }

        for (int i = 0; i < count;) {
            Buffer buffer(m_MemOffset,
                          (unsigned char *) m_MemDataEndPos - (unsigned char *) m_MemOffset,
                          BufferNoCopy);
            PBEncodeItem item = MiniPBCoder::DecodeItem(buffer);
            long itemSize = MiniPBCoder::CalculatedSize(item);
            if (itemSize == 0) {
                break;
            }
            if (itemSize + (unsigned char *) m_MemOffset > (unsigned char *) m_MemDataEndPos) {
                break;
            }

            m_MemOffset = (unsigned char *) m_MemOffset + itemSize;

            std::string dateStr;
            dateStr.append((const char *) item.date.GetBegin(), item.date.Length());
            std::int64_t dateInItem = StrToInt64(dateStr);

            std::shared_ptr<CacheItem> cacheItem(new CacheItem);
            cacheItem->pbEncodeItem = std::move(item);
            if (m_MemOffset >= m_MemDataEndPos) {
                if (IsExpired(dateInItem, expiredTime)) {
                    if (!ret.empty()) {
                        ret.back()->fromMem = m_MemOffset;
                    }
                } else {
                    cacheItem->fromMem = m_MemOffset;
                    ret.push_back(cacheItem);
                }
                break;
            }

            //过期数据
            if (IsExpired(dateInItem, expiredTime)) {
                continue;
            }
            i++;
            ret.push_back(cacheItem);
        }
        return ret;
    }

    void DataProvider::ClearFile(const std::string &path) {
        if (m_FileInputStream != NULL && path == m_FileInputStream->GetPath()) {
            if (m_FileInputStream->IsOpened()) {
                m_FileInputStream->Close();
            }
            m_FileInputStream = NULL;
            m_Files.pop_front();
        }
        File::RemoveFile(path);
        m_UploadingFile.erase(File::GetFileNameWithExt(path));
    }

    void DataProvider::ClearMem() {
        m_FromMem->Clear();
        m_MemOffset = m_FromMem->GetBegin();
        m_MemDataEndPos = m_MemOffset;
        m_IsUploadingMem = false;
    }

    std::list<std::string> DataProvider::ListFiles() {
        std::list<std::string> files = File::FileList(m_FromPath);
        files = File::FilterByFun(files, [this](const std::string &fileName) {
            if (EndsWith(fileName, Reporter::DATA_SUFFIX)) {
                if (m_UploadingFile.find(fileName) == m_UploadingFile.end()) {
                    return true;
                }
            }
            return false;
        });
        files.sort([](std::string str1, std::string str2) {
            std::string str1Name = File::GetFileName(str1);
            std::string str2Name = File::GetFileName(str2);
            if (StrToInt64(str1Name) < StrToInt64(str2Name)) {
                return true;
            }
            return false;
        });
        return files;
    }
}
