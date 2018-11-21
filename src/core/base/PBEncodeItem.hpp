#ifndef PBENCODEITEM_HPP
#define PBENCODEITEM_HPP

#include "Buffer.h"
#include <cstdint>
#include <memory.h>
#include <string>

namespace future {

    enum PBEncodeItemType {
        PBEncodeItemType_String = 1,
        PBEncodeItemType_Data = 2,
    };

    struct PBEncodeItem {
        PBEncodeItemType type;
        uint32_t data_len;
        uint32_t crc;
        uint32_t extra_data_len;
        Buffer data;
        Buffer extra_data;

        PBEncodeItem() : type(PBEncodeItemType_Data), crc(0), data_len(0), extra_data_len(0),
                         data(), extra_data() {
        }
    };

}
#endif //PBENCODEITEM_HPP
