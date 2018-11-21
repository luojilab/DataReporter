#ifndef CODEDOUTPUTDATA_H
#define CODEDOUTPUTDATA_H

#include "Buffer.h"
#include <cstdint>
#include <string>

namespace future {

    class RawOutput {

    public:
        RawOutput(void *ptr, size_t len);

        ~RawOutput();

        int32_t SpaceLeft();

        void Seek(size_t addedSize);

        void WriteRawByte(uint8_t value);

        void WriteFixed32(int32_t value);

        void WriteRawVarint32(int32_t value);

        void WriteRawVarint64(int64_t value);

        void WriteRawData(const Buffer &data);

        void WriteDouble(double value);

        void WriteFloat(float value);

        void WriteInt64(int64_t value);

        void WriteInt32(int32_t value);

        void WriteBool(bool value);

        void WriteString(const std::string &value);

        void WriteData(const Buffer &value);

    private:
        void WriteRawLittleEndian32(int32_t value);

        void WriteRawLittleEndian64(int64_t value);

    private:
        uint8_t *m_Ptr;
        size_t m_Size;
        int32_t m_Position;
    };

}
#endif //MMKV_CODEDOUTPUTDATA_H
