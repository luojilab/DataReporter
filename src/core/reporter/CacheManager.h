//
// Created by bingjian on 2018/11/6.
//

#ifndef DATA_REPORTER_CACHEMANAGER_H
#define DATA_REPORTER_CACHEMANAGER_H

#include <list>
#include <map>
#include <cstddef>
#include <string>
#include <memory>
#include <functional>

#include "MmapedFile.h"
#include "PBEncodeItem.hpp"
#include "FileInputStream.h"
#include "CacheItem.h"

namespace future {

    class CacheManager {
    public:
        CacheManager(const std::string &fromPath, std::shared_ptr<Buffer> &fromMem,
                     std::function<std::int64_t(void *, int)> updateMem);

        ~CacheManager();

        std::list<std::shared_ptr<CacheItem> > ReadData(std::size_t count);

        void ClearFile(const std::string &path);

        void ClearMem();

    private:
        std::list<std::string> ListFiles();

        std::list<std::shared_ptr<CacheItem> > ReadFromMem(std::size_t count);

        std::list<std::shared_ptr<CacheItem> > ReadFromFile(std::size_t count);

    private:
        std::string m_FromPath;
        std::list<std::string> m_Files;
        std::map<std::string, int> m_UploadingFile;
        std::shared_ptr<FileInputStream> m_FileInputStream;
        std::shared_ptr<Buffer> m_FromMem;
        std::function<std::int64_t(void *, int)> m_UpdateMem;
        void *m_MemDataEndPos;
        void *m_MemOffset;
        bool m_IsUploadingMem;
    };

}
#endif //DATA_REPORTER_CACHEMANAGER_H
