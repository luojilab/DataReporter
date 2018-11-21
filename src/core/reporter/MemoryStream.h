//
// Created by bingjian on 2018/11/6.
//

#ifndef DATA_REPORTER_MEMCACHE_H
#define DATA_REPORTER_MEMCACHE_H

#include <list>
#include <mutex>
#include <memory>
#include "MmapedFile.h"
#include "Buffer.h"

namespace future {

    class MemoryStream {

    public:
        MemoryStream(const std::shared_ptr<Buffer> &buffer);

        ~MemoryStream();

        void Write(const std::string &data);

        void MoveToFile(const std::string &path, void *safeBuf);

        std::int64_t MoveToMem(void *mem, long maxLen);

        std::size_t GetOffset();

        void Seek(int offset);

        void Clear(std::size_t offset);

    private:

        unsigned char *m_PosPtr;
        std::size_t m_Len;
        std::shared_ptr<Buffer> m_Buffer;
        std::mutex m_Mut;
    };
}

#endif //DATA_REPORTER_MEMCACHE_H
