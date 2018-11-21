#ifndef METAINFO_H
#define METAINFO_H

#include <cassert>
#include <cstdint>
#include <cstring>
#include <assert.h>

namespace future {

    struct MetaInfo {
        uint32_t m_crcDigest = 0;
        uint32_t m_version = 1;
        uint32_t m_sequence = 0; // full write-back count

        void write(void *ptr) {
            assert(ptr);
            memcpy(ptr, this, sizeof(MetaInfo));
        }

        void read(const void *ptr) {
            assert(ptr);
            memcpy(this, ptr, sizeof(MetaInfo));
        }
    };

}
#endif //METAINFO_H
