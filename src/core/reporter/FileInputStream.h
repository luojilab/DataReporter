//
// Created by bingjian on 2018/11/6.
//

#ifndef DATA_REPORTER_FILECACHE_H
#define DATA_REPORTER_FILECACHE_H

#include <cstddef>
#include <string>
#include <list>
#include <memory>
#include "CacheItem.h"

namespace future {

    class FileInputStream {

    public:

        FileInputStream(const std::string &path);

        ~FileInputStream();

        bool Open();

        bool IsOpened();

        void Seek(long offset);

        long Count();

        void Close();

        const std::string &GetPath() {
            return m_Path;
        }

        std::list<std::shared_ptr<CacheItem> > ReadData(std::size_t count, std::int64_t expiredTime);

    private:
        FILE *m_Fp;
        std::string m_Path;
        long m_Offset;
        bool m_IsOpened;
    };
}

#endif //DATA_REPORTER_FILECACHE_H
