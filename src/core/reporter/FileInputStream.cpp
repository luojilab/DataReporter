//
// Created by bingjian on 2018/11/6.
//

#include <MiniPBCoder.h>
#include "FileInputStream.h"


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

    std::list<std::shared_ptr<CacheItem> > FileInputStream::ReadData(std::size_t count) {
        std::list<std::shared_ptr<CacheItem> > ret;
        long fileSize = Count();
        for (int i = 0; i < count; i++) {
            PBEncodeItem pbEncodeItem;
            if (!MiniPBCoder::DecodeItem(pbEncodeItem, m_Fp, m_Offset)) {
                return std::move(ret);
            }

            if (!MiniPBCoder::VerifyItem(pbEncodeItem)) {
                return std::move(ret);
            }

            std::shared_ptr<CacheItem> cacheItem(new CacheItem);
            cacheItem->pbEncodeItem = std::move(pbEncodeItem);

            if (m_Offset >= fileSize) {
                cacheItem->fromPath = m_Path;
            }
            ret.push_back(cacheItem);
        }
        return std::move(ret);
    }
}