#ifndef CODEDINPUTDATA_H
#define CODEDINPUTDATA_H

#include "Buffer.h"
#include <cstdint>
#include <string>

namespace future {

    class RawInput {
    public:
        RawInput(const void *oData, int32_t length);

        ~RawInput();

        bool ReadBool();

        double ReadDouble();

        float ReadFloat();

        int64_t ReadInt64();

        int32_t ReadInt32();

        int32_t ReadFixed32();

        std::string ReadString();

        Buffer ReadData();

        Buffer ReadData(int32_t count);

    private:
        int8_t ReadRawByte();

        int32_t ReadRawVarint32();

        int32_t ReadRawLittleEndian32();

        int64_t ReadRawLittleEndian64();

    private:
        uint8_t *m_Ptr;
        int32_t m_Size;
        int32_t m_Position;
    };

}
#endif //MMKV_CODEDINPUTDATA_H
