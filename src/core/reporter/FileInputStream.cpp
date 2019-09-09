//
// Created by bingjian on 2018/11/6.
//

#include <MiniPBCoder.h>
#include "FileInputStream.h"
#include "DataProvider.h"
#include "StringUtil.h"


namespace future {

    FileInputStream::FileInputStream(const std::string &path) : m_Offset(0), m_Path(path),
                                                                m_Fp(NULL), m_IsOpened(false) {

    }

    FileInputStream::~FileInputStream() {

    }

    bool FileInputStream::Open() {
        m_Fp = fopen(m_Path.c_str(), "rb");
        if (m_Fp != NULL) {
            m_IsOpened = true;
            return true;
        }
        return false;
    }

    bool FileInputStream::IsOpened() {
        return m_IsOpened;
    }

    void FileInputStream::Seek(long offset) {
        if (m_Fp != NULL) {
            fseek(m_Fp, offset, SEEK_SET);
            m_Offset = offset;
        }
    }

    long FileInputStream::Count() {
        if (m_Fp == NULL) {
            return 0;
        }
        long pos = ftell(m_Fp);
        fseek(m_Fp, 0, SEEK_END);
        long size = ftell(m_Fp);
        fseek(m_Fp, pos, SEEK_SET);
        return size;
    }

    void FileInputStream::Close() {
        if (m_Fp != NULL) {
            fclose(m_Fp);
            m_Fp = NULL;
        }
        m_IsOpened = false;
        m_Offset = 0;
    }

    std::shared_ptr<std::list<std::shared_ptr<CacheItem> > >
    FileInputStream::ReadData(std::size_t count, std::int64_t expiredTime) {
        std::shared_ptr<std::list<std::shared_ptr<CacheItem> > > ret = std::make_shared<std::list<std::shared_ptr<CacheItem> > >();
        long fileSize = Count();
        for (int i = 0; i < count;) {
            PBEncodeItem pbEncodeItem;
            if (!MiniPBCoder::DecodeItem(pbEncodeItem, m_Fp, m_Offset)) {
                return ret;
            }

            if (!MiniPBCoder::VerifyItem(pbEncodeItem)) {
                return ret;
            }

            std::shared_ptr<CacheItem> cacheItem(new CacheItem);
            cacheItem->pbEncodeItem = std::move(pbEncodeItem);

            std::string dateStr;
            dateStr.append((const char *) cacheItem->pbEncodeItem.date.GetBegin(),
                           cacheItem->pbEncodeItem.date.Length());
            std::int64_t dateInItem = StrToInt64(dateStr);
            if (m_Offset >= fileSize) {
                if (DataProvider::IsExpired(dateInItem, expiredTime)) {
                    if (!ret->empty()) {
                        ret->back()->fromPath = m_Path;
                    }
                    i++;
                    continue;
                } else {
                    cacheItem->fromPath = m_Path;
                }
            }
            if (DataProvider::IsExpired(dateInItem, expiredTime)) {
                continue;
            }

            i++;
            ret->push_back(cacheItem);
        }
        return ret;
    }
}