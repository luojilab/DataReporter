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

    const u_int16_t HEADER_ID = 0x5244;
    const int32_t VERSION_1 = 1;
    const int32_t VERSION_2 = 2;

    MiniPBCoder::MiniPBCoder() {
    }

    MiniPBCoder::~MiniPBCoder() {
    }

    PBEncodeItem MiniPBCoder::DecodeItem(const Buffer &buffer, int32_t &decode_len) {
        MiniPBCoder oCoder;
        return oCoder.DecodeData(buffer, decode_len);
    }

    bool MiniPBCoder::DecodeItem(PBEncodeItem &out, FILE *fp, long &offset) {
        MiniPBCoder oCoder;
        return oCoder.DecodeData(out, fp, offset);
    }

    std::size_t MiniPBCoder::CalculatedSize(const PBEncodeItem &encodeItem) {
        if (encodeItem.head_id != HEADER_ID || encodeItem.version == 0 || encodeItem.crc == 0) {
            return 0;
        }
        std::size_t ret = 0;
        ret += PBEncodeItemHeadLen();
        ret += encodeItem.data_len;
        ret += encodeItem.date_len;
        return ret;
    }

    std::size_t MiniPBCoder::CalculatedSize(const std::size_t dataLen) {
        std::size_t ret = 0;
        ret += PBEncodeItemHeadLen();
        ret += dataLen;
        return ret;
    }

    long MiniPBCoder::PBEncodeItemHeadLen() {
        long ret = 0;
        PBEncodeItem pbEncodeItem;
        ret += sizeof(pbEncodeItem.head_id);
        ret += sizeof(pbEncodeItem.version);
        ret += sizeof(pbEncodeItem.crypto_flag);
        ret += sizeof(pbEncodeItem.crc);
        ret += sizeof(pbEncodeItem.data_len);
        ret += sizeof(pbEncodeItem.date_len);
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

    PBEncodeItem MiniPBCoder::DecodeData(const Buffer &buffer, int32_t &decode_len) {
        PBEncodeItem encodeItem;
        decode_len = 0;
        RawInput rawInput(buffer.GetBegin(),
                          static_cast<int32_t>(buffer.Length()));

        uint16_t head_id = rawInput.ReadFixed16();
        decode_len += 2;
        uint16_t version = rawInput.ReadFixed16();
        decode_len += 2;
        if (head_id == HEADER_ID && version == VERSION_2) {
            encodeItem.head_id = head_id;
            encodeItem.version = version;
            encodeItem.crypto_flag = rawInput.ReadFixed32();
            decode_len += 4;
            encodeItem.data_len = rawInput.ReadFixed32();
            decode_len += 4;
            encodeItem.crc = rawInput.ReadFixed32();
            decode_len += 4;
            encodeItem.date_len = rawInput.ReadFixed32();
            decode_len += 4;
            if (encodeItem.data_len != 0) {
                encodeItem.data = std::move(rawInput.ReadData(encodeItem.data_len));
                decode_len += encodeItem.data_len;
            }

            if (encodeItem.date_len != 0) {
                encodeItem.date = std::move(rawInput.ReadData(encodeItem.date_len));
                decode_len += encodeItem.date_len;
            }
        } else if (head_id == VERSION_1) {
            encodeItem.head_id = HEADER_ID;
            encodeItem.version = VERSION_1;
            encodeItem.crypto_flag = 0;
            encodeItem.data_len = rawInput.ReadFixed32();
            decode_len += 4;
            encodeItem.crc = rawInput.ReadFixed32();
            decode_len += 4;
            encodeItem.date_len = rawInput.ReadFixed32();
            decode_len += 4;
            if (encodeItem.data_len != 0) {
                encodeItem.data = std::move(rawInput.ReadData(encodeItem.data_len));
                decode_len += encodeItem.data_len;
            }

            if (encodeItem.date_len != 0) {
                encodeItem.date = std::move(rawInput.ReadData(encodeItem.date_len));
                decode_len += encodeItem.date_len;
            }
        }

        return std::move(encodeItem);
    }

    bool MiniPBCoder::DecodeData(PBEncodeItem &out, FILE *fp, long &offset) {
        if (fp == NULL) {
            return false;
        }

        unsigned char buf[256];
        memset(buf, 0, sizeof(buf));
        uint32_t read_len = 0;
        read_len = fread(buf, 1, sizeof(out.head_id) + sizeof(out.version), fp);
        offset += read_len;
        if (read_len != sizeof(out.head_id) + sizeof(out.version)) {
            return false;
        }
        Buffer buffer(buf, read_len, BufferNoCopy);
        RawInput rawInput(buffer.GetBegin(),
                          static_cast<int32_t>(buffer.Length()));
        uint16_t head_id = rawInput.ReadFixed16();
        uint16_t version = rawInput.ReadFixed16();
        if (head_id == HEADER_ID && version == VERSION_2) {
            out.head_id = head_id;
            out.version = version;
            uint32_t head_len =
                    sizeof(out.crypto_flag) + sizeof(out.data_len) +
                    sizeof(out.crc) + sizeof(out.date_len);
            read_len = fread(buf, 1, head_len, fp);
            offset += read_len;
            if (read_len != head_len) {
                return false;
            }
            Buffer buffer(buf, read_len, BufferNoCopy);
            RawInput rawInput(buffer.GetBegin(),
                              static_cast<int32_t>(buffer.Length()));

            out.crypto_flag = rawInput.ReadFixed32();
            out.data_len = rawInput.ReadFixed32();
            out.crc = rawInput.ReadFixed32();
            out.date_len = rawInput.ReadFixed32();


        } else if (head_id == VERSION_1) {
            out.head_id = HEADER_ID;
            out.version = VERSION_1;
            out.crypto_flag = 0;

            uint32_t head_len = sizeof(out.data_len) + sizeof(out.crc) + sizeof(out.date_len);
            read_len = fread(buf, 1, head_len, fp);
            offset += read_len;
            if (read_len != head_len) {
                return false;
            }
            Buffer buffer(buf, read_len, BufferNoCopy);
            RawInput rawInput(buffer.GetBegin(),
                              static_cast<int32_t>(buffer.Length()));

            out.data_len = rawInput.ReadFixed32();
            out.crc = rawInput.ReadFixed32();
            out.date_len = rawInput.ReadFixed32();

        }

        if (out.data_len != 0) {
            Buffer data(out.data_len);
            read_len = fread(data.GetBegin(), 1, data.Length(), fp);
            offset += read_len;
            if (out.data_len != read_len) {
                return false;
            }

            out.data = std::move(data);
        }

        if (out.date_len != 0) {
            Buffer extra_data(out.date_len);
            read_len = fread(extra_data.GetBegin(), 1, extra_data.Length(), fp);
            offset += read_len;
            if (out.date_len != read_len) {
                return false;
            }
            out.date = std::move(extra_data);
        }
        return true;
    }

    Buffer MiniPBCoder::EncodeData(const Buffer &buffer, int cryptoFlag, const std::string &date) {
        PBEncodeItem encodeItem;
        encodeItem.head_id = HEADER_ID;
        encodeItem.version = VERSION_2;
        encodeItem.crypto_flag = cryptoFlag;
        Buffer data((void *) buffer.GetBegin(), buffer.Length(), BufferCopy);
        encodeItem.data = std::move(data);
        encodeItem.data_len = buffer.Length();
        encodeItem.crc = (uint32_t) crc32(0, (const Bytef *) encodeItem.data.GetBegin(),
                                          (uint32_t) encodeItem.data_len);
        encodeItem.date_len = date.length();
        Buffer dateBuf((void *) date.data(), encodeItem.date_len, BufferCopy);
        encodeItem.date = std::move(dateBuf);
        std::size_t count = CalculatedSize(encodeItem);
        Buffer outBuffer(count);
        WriteItem(outBuffer, encodeItem);
        return std::move(outBuffer);
    }

    void MiniPBCoder::WriteItem(Buffer &buffer, const PBEncodeItem &encodeItem) {
        RawOutput rawOutput(buffer.GetBegin(), buffer.Length());
        rawOutput.WriteFixed16(encodeItem.head_id);
        rawOutput.WriteFixed16(encodeItem.version);
        rawOutput.WriteFixed32(encodeItem.crypto_flag);
        rawOutput.WriteFixed32(encodeItem.data_len);
        rawOutput.WriteFixed32(encodeItem.crc);
        rawOutput.WriteFixed32(encodeItem.date_len);
        rawOutput.WriteRawData(encodeItem.data);
        rawOutput.WriteRawData(encodeItem.date);
    }

}