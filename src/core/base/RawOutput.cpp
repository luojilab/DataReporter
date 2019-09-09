#include <assert.h>
#include "RawOutput.h"
#include "Log.h"
#include "PBUtility.h"

namespace future {

    RawOutput::RawOutput(void *ptr, size_t len)
            : m_Ptr((uint8_t *) ptr), m_Size(len), m_Position(0) {
        assert(m_Ptr);
    }

    RawOutput::~RawOutput() {
        m_Ptr = nullptr;
        m_Position = 0;
    }

    void RawOutput::WriteDouble(double value) {
        this->WriteRawLittleEndian64(Float64ToInt64(value));
    }

    void RawOutput::WriteFloat(float value) {
        this->WriteRawLittleEndian32(Float32ToInt32(value));
    }

    void RawOutput::WriteInt64(int64_t value) {
        this->WriteRawVarint64(value);
    }

    void RawOutput::WriteInt32(int32_t value) {
        if (value >= 0) {
            this->WriteRawVarint32(value);
        } else {
            this->WriteRawVarint64(value);
        }
    }

    void RawOutput::WriteBool(bool value) {
        this->WriteRawByte(static_cast<uint8_t>(value ? 1 : 0));
    }

    void RawOutput::WriteString(const std::string &value) {
        size_t numberOfBytes = value.size();
        this->WriteRawVarint32((int32_t) numberOfBytes);
        memcpy(m_Ptr + m_Position, ((uint8_t *) value.data()), value.size());
        m_Position += numberOfBytes;
    }

    void RawOutput::WriteData(const Buffer &value) {
        this->WriteRawVarint32((int32_t) value.Length());
        this->WriteRawData(value);
    }

    int32_t RawOutput::SpaceLeft() {
        return int32_t(m_Size - m_Position);
    }

    void RawOutput::Seek(size_t addedSize) {
        m_Position += addedSize;

        if (m_Position > m_Size) {
            Error("OutOfSpace");
        }
    }

    void RawOutput::WriteRawByte(uint8_t value) {
        if (m_Position == m_Size) {
            Error("m_Position: %d, m_Size: %zd", m_Position, m_Size);
            return;
        }

        m_Ptr[m_Position++] = value;
    }

    void RawOutput::WriteFixed16(u_int16_t value){
        WriteRawLittleEndian16(value);
    }

    void RawOutput::WriteFixed32(int32_t value) {
        WriteRawLittleEndian32(value);
    }

    void RawOutput::WriteRawData(const Buffer &data) {
        size_t numberOfBytes = data.Length();
        memcpy(m_Ptr + m_Position, data.GetBegin(), numberOfBytes);
        m_Position += numberOfBytes;
    }

    void RawOutput::WriteRawVarint32(int32_t value) {
        while (true) {
            if ((value & ~0x7f) == 0) {
                this->WriteRawByte(static_cast<uint8_t>(value));
                return;
            } else {
                this->WriteRawByte(static_cast<uint8_t>((value & 0x7F) | 0x80));
                value = LogicalRightShift32(value, 7);
            }
        }
    }

    void RawOutput::WriteRawVarint64(int64_t value) {
        while (true) {
            if ((value & ~0x7f) == 0) {
                this->WriteRawByte(static_cast<uint8_t>(value));
                return;
            } else {
                this->WriteRawByte(static_cast<uint8_t>((value & 0x7f) | 0x80));
                value = LogicalRightShift64(value, 7);
            }
        }
    }

    void RawOutput::WriteRawLittleEndian16(u_int16_t value){
        this->WriteRawByte(static_cast<uint8_t>((value) & 0xff));
        this->WriteRawByte(static_cast<uint8_t>((value >> 8) & 0xff));
    }

    void RawOutput::WriteRawLittleEndian32(int32_t value) {
        this->WriteRawByte(static_cast<uint8_t>((value) & 0xff));
        this->WriteRawByte(static_cast<uint8_t>((value >> 8) & 0xff));
        this->WriteRawByte(static_cast<uint8_t>((value >> 16) & 0xff));
        this->WriteRawByte(static_cast<uint8_t>((value >> 24) & 0xff));
    }

    void RawOutput::WriteRawLittleEndian64(int64_t value) {
        this->WriteRawByte(static_cast<uint8_t>((value) & 0xff));
        this->WriteRawByte(static_cast<uint8_t>((value >> 8) & 0xff));
        this->WriteRawByte(static_cast<uint8_t>((value >> 16) & 0xff));
        this->WriteRawByte(static_cast<uint8_t>((value >> 24) & 0xff));
        this->WriteRawByte(static_cast<uint8_t>((value >> 32) & 0xff));
        this->WriteRawByte(static_cast<uint8_t>((value >> 40) & 0xff));
        this->WriteRawByte(static_cast<uint8_t>((value >> 48) & 0xff));
        this->WriteRawByte(static_cast<uint8_t>((value >> 56) & 0xff));
    }

}