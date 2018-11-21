#include <string>
#include <sys/stat.h>
#include <vector>
#include <zlib.h>
#include "MiniPBCoder.h"
#include "RawInput.h"
#include "RawOutput.h"
#include "Buffer.h"
#include "PBEncodeItem.hpp"
#include "PBUtility.h"

namespace future {

    MiniPBCoder::MiniPBCoder() {
    }

    MiniPBCoder::~MiniPBCoder() {
    }

    PBEncodeItem MiniPBCoder::DecodeItem(const Buffer &buffer) {
        MiniPBCoder oCoder;
        return oCoder.DecodeData(buffer);
    }

    bool MiniPBCoder::DecodeItem(PBEncodeItem &out, FILE *fp, long &offset) {
        MiniPBCoder oCoder;
        return oCoder.DecodeData(out, fp, offset);
    }

    std::size_t MiniPBCoder::CalculatedSize(const PBEncodeItem &encodeItem) {
        if (encodeItem.type == 0 || encodeItem.crc == 0) {
            return 0;
        }
        std::size_t ret = 0;
        ret += PBEncodeItemHeadLen();
        ret += encodeItem.data_len;
        ret += encodeItem.extra_data_len;
        return ret;
    }

    std::size_t MiniPBCoder::CalculatedSize(const std::string &data) {
        std::size_t ret = 0;
        ret += PBEncodeItemHeadLen();
        ret += data.length();
        return ret;
    }

    long MiniPBCoder::PBEncodeItemHeadLen() {
        long ret = 0;
        PBEncodeItem pbEncodeItem;
        ret += sizeof(pbEncodeItem.type);
        ret += sizeof(pbEncodeItem.crc);
        ret += sizeof(pbEncodeItem.data_len);
        ret += sizeof(pbEncodeItem.extra_data_len);
        return ret;
    }

    bool MiniPBCoder::VerifyItem(const PBEncodeItem &encodeItem) {
        if (encodeItem.data_len == 0 || encodeItem.crc == 0) {
            return false;
        }
        uint32_t itemCrc = (uint32_t) crc32(0,
                                            (const Bytef *) encodeItem.data.GetBegin(),
                                            (uint32_t) encodeItem.data_len);
        if (itemCrc == encodeItem.crc) {
            return true;
        }
        return false;
    }

    PBEncodeItem MiniPBCoder::DecodeData(const Buffer &buffer) {
        PBEncodeItem encodeItem;
        int headLen = PBEncodeItemHeadLen();
        if (buffer.Length() < headLen) {
            return std::move(encodeItem);
        }
        RawInput rawInput(buffer.GetBegin(),
                          static_cast<int32_t>(buffer.Length()));

        encodeItem.type = (PBEncodeItemType) rawInput.ReadFixed32();
        encodeItem.data_len = rawInput.ReadFixed32();
        encodeItem.crc = rawInput.ReadFixed32();
        encodeItem.extra_data_len = rawInput.ReadFixed32();
        if (encodeItem.data_len != 0) {
            encodeItem.data = std::move(rawInput.ReadData(encodeItem.data_len));
        }

        if (encodeItem.extra_data_len != 0) {
            encodeItem.extra_data = std::move(rawInput.ReadData(encodeItem.extra_data_len));
        }
        return std::move(encodeItem);
    }

    bool MiniPBCoder::DecodeData(PBEncodeItem &out, FILE *fp, long &offset) {
        if (fp == NULL) {
            return false;
        }
        int headLen = PBEncodeItemHeadLen();
        unsigned char *headBuf = new unsigned char[headLen];
        if (headLen != fread(headBuf, 1, headLen, fp)) {
            offset += headLen;
            delete[] headBuf;
            return false;
        }
        offset += headLen;
        Buffer buffer(headBuf, headLen, BufferNoCopy);
        RawInput rawInput(buffer.GetBegin(),
                          static_cast<int32_t>(buffer.Length()));
        out.type = (PBEncodeItemType) rawInput.ReadFixed32();
        out.data_len = rawInput.ReadFixed32();
        out.crc = rawInput.ReadFixed32();
        out.extra_data_len = rawInput.ReadFixed32();
        delete[] headBuf;
        if (out.data_len != 0) {
            Buffer data(out.data_len);
            if (out.data_len != fread(data.GetBegin(), 1, data.Length(), fp)) {
                offset += out.data_len;
                delete[] headBuf;
                return false;
            }

            offset += out.data_len;
            out.data = std::move(data);
        }

        if (out.extra_data_len != 0) {
            Buffer extra_data(out.extra_data_len);
            if (out.extra_data_len != fread(extra_data.GetBegin(), 1, extra_data.Length(), fp)) {
                offset += out.extra_data_len;
                return false;
            }
            offset += out.extra_data_len;
            out.extra_data = std::move(extra_data);
        }
        return true;
    }

    Buffer MiniPBCoder::EncodeData(const std::string &str) {
        PBEncodeItem encodeItem;
        encodeItem.type = PBEncodeItemType_String;
        encodeItem.data_len = static_cast<int32_t>(str.size());
        Buffer data((void *) str.data(), encodeItem.data_len);
        encodeItem.data = std::move(data);
        encodeItem.crc = (uint32_t) crc32(0, (const Bytef *) encodeItem.data.GetBegin(),
                                          (uint32_t) encodeItem.data_len);
        encodeItem.extra_data_len = 0;
        std::size_t count = CalculatedSize(encodeItem);
        Buffer outBuffer(count);
        WriteItem(outBuffer, encodeItem);

        return std::move(outBuffer);
    }

    Buffer MiniPBCoder::EncodeData(const Buffer &buffer) {
        PBEncodeItem encodeItem;
        encodeItem.type = PBEncodeItemType_Data;
        Buffer data((void *) buffer.GetBegin(), buffer.Length());
        encodeItem.data = std::move(data);
        encodeItem.data_len = buffer.Length();
        encodeItem.crc = (uint32_t) crc32(0, (const Bytef *) encodeItem.data.GetBegin(),
                                          (uint32_t) encodeItem.data_len);
        encodeItem.extra_data_len = 0;
        std::size_t count = CalculatedSize(encodeItem);
        Buffer outBuffer(count);
        WriteItem(outBuffer, encodeItem);
        return std::move(outBuffer);
    }

    void MiniPBCoder::WriteItem(Buffer &buffer, const PBEncodeItem &encodeItem) {
        RawOutput rawOutput(buffer.GetBegin(), buffer.Length());
        rawOutput.WriteFixed32(encodeItem.type);
        rawOutput.WriteFixed32(encodeItem.data_len);
        rawOutput.WriteFixed32(encodeItem.crc);
        rawOutput.WriteFixed32(encodeItem.extra_data_len);
        rawOutput.WriteRawData(encodeItem.data);
        rawOutput.WriteRawData(encodeItem.extra_data);
    }

}