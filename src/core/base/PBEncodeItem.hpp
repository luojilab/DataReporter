#ifndef PBENCODEITEM_HPP
#define PBENCODEITEM_HPP

#include "Buffer.h"
#include <cstdint>
#include <memory.h>
#include <string>

namespace future {

    struct PBEncodeItem {
        u_int16_t head_id;
        u_int16_t version;
        uint32_t crypto_flag;
        uint32_t data_len;
        uint32_t crc;
        uint32_t date_len;
        Buffer data;
        Buffer date;

        PBEncodeItem() : head_id(0), version(2), crypto_flag(0), crc(0), data_len(0), date_len(0),
                         data(), date() {
        }
    };

}
#endif //PBENCODEITEM_HPP
