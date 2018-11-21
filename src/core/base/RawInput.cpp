#include <limits.h>
#include <assert.h>
#include "RawInput.h"
#include "Log.h"
#include "PBUtility.h"

namespace future {

    RawInput::RawInput(const void *oData, int32_t length)
            : m_Ptr((uint8_t *) oData), m_Size(length), m_Position(0) {
        assert(m_Ptr);
    }

    RawInput::~RawInput() {
        m_Ptr = nullptr;
        m_Size = 0;
    }

    double RawInput::ReadDouble() {
        return Int64ToFloat64(this->ReadRawLittleEndian64());
    }

    float RawInput::ReadFloat() {
        return Int32ToFloat32(this->ReadRawLittleEndian32());
    }

    int64_t RawInput::ReadInt64() {
        int32_t shift = 0;
        int64_t result = 0;
        while (shift < 64) {
            int8_t b = this->ReadRawByte();
            result |= (int64_t) (b & 0x7f) << shift;
            if ((b & 0x80) == 0) {
                return result;
            }
            shift += 7;
        }
        Error("InvalidProtocolBuffer malformedInt64");
        return 0;
    }

    int32_t RawInput::ReadInt32() {
        return this->ReadRawVarint32();
    }

    int32_t RawInput::ReadFixed32() {
        return this->ReadRawLittleEndian32();
    }

    bool RawInput::ReadBool() {
        return this->ReadRawVarint32() != 0;
    }

    std::string RawInput::ReadString() {
        int32_t size = this->ReadRawVarint32();
        if (size <= (m_Size - m_Position) && size > 0) {
            std::string result((char *) (m_Ptr + m_Position), size);
            m_Position += size;
            return result;
        } else if (size == 0) {
            return "";
        } else {
            Error("Invalid Size: %d", size);
            return "";
        }
    }

    Buffer RawInput::ReadData() {
        int32_t size = this->ReadRawVarint32();
        if (size < 0) {
            Error("InvalidProtocolBuffer negativeSize");
            return Buffer(0);
        }

        if (size <= m_Size - m_Position) {
            Buffer data(((int8_t *) m_Ptr) + m_Position, size);
            m_Position += size;
            return data;
        } else {
            Error("InvalidProtocolBuffer truncatedMessage");
            return Buffer(0);
        }
    }

    Buffer RawInput::ReadData(int32_t count){
        if (count < 0) {
            Error("InvalidProtocolBuffer negativeSize");
            return Buffer(0);
        }

        if (count <= m_Size - m_Position) {
            Buffer data(((int8_t *) m_Ptr) + m_Position, count);
            m_Position += count;
            return data;
        } else {
            Error("InvalidProtocolBuffer truncatedMessage");
            return Buffer(0);
        }
    }

    int32_t RawInput::ReadRawVarint32() {
        int8_t tmp = this->ReadRawByte();
        if (tmp >= 0) {
            return tmp;
        }
        int32_t result = tmp & 0x7f;
        if ((tmp = this->ReadRawByte()) >= 0) {
            result |= tmp << 7;
        } else {
            result |= (tmp & 0x7f) << 7;
            if ((tmp = this->ReadRawByte()) >= 0) {
                result |= tmp << 14;
            } else {
                result |= (tmp & 0x7f) << 14;
                if ((tmp = this->ReadRawByte()) >= 0) {
                    result |= tmp << 21;
                } else {
                    result |= (tmp & 0x7f) << 21;
                    result |= (tmp = this->ReadRawByte()) << 28;
                    if (tmp < 0) {
                        // discard upper 32 bits
                        for (int i = 0; i < 5; i++) {
                            if (this->ReadRawByte() >= 0) {
                                return result;
                            }
                        }
                        Error("InvalidProtocolBuffer malformed varint32");
                    }
                }
            }
        }
        return result;
    }

    int32_t RawInput::ReadRawLittleEndian32() {
        int8_t b1 = this->ReadRawByte();
        int8_t b2 = this->ReadRawByte();
        int8_t b3 = this->ReadRawByte();
        int8_t b4 = this->ReadRawByte();
        return (((int32_t) b1 & 0xff)) | (((int32_t) b2 & 0xff) << 8) |
               (((int32_t) b3 & 0xff) << 16) |
               (((int32_t) b4 & 0xff) << 24);
    }

    int64_t RawInput::ReadRawLittleEndian64() {
        int8_t b1 = this->ReadRawByte();
        int8_t b2 = this->ReadRawByte();
        int8_t b3 = this->ReadRawByte();
        int8_t b4 = this->ReadRawByte();
        int8_t b5 = this->ReadRawByte();
        int8_t b6 = this->ReadRawByte();
        int8_t b7 = this->ReadRawByte();
        int8_t b8 = this->ReadRawByte();
        return (((int64_t) b1 & 0xff)) | (((int64_t) b2 & 0xff) << 8) |
               (((int64_t) b3 & 0xff) << 16) |
               (((int64_t) b4 & 0xff) << 24) | (((int64_t) b5 & 0xff) << 32) |
               (((int64_t) b6 & 0xff) << 40) | (((int64_t) b7 & 0xff) << 48) |
               (((int64_t) b8 & 0xff) << 56);
    }

    int8_t RawInput::ReadRawByte() {
        if (m_Position == m_Size) {
            Error("reach end, m_Position: %d, m_Size: %d", m_Position, m_Size);
            return 0;
        }
        int8_t *bytes = (int8_t *) m_Ptr;
        return bytes[m_Position++];
    }

}