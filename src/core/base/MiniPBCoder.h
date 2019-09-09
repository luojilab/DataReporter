#ifndef MINIPBCODER_H
#define MINIPBCODER_H

#include "Buffer.h"
#include "Log.h"
#include "PBEncodeItem.hpp"
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace future {

    class RawInput;

    class RawOutput;

    class MiniPBCoder {

    public:
        static Buffer BuildEncodeData(const Buffer &buffer, int cryptoFlag, const std::string &date) {
            MiniPBCoder pbcoder;
            return pbcoder.EncodeData(buffer, cryptoFlag, date);
        }

        static PBEncodeItem DecodeItem(const Buffer &buffer, int32_t &decode_len);

        static bool DecodeItem(PBEncodeItem &out, FILE *fp, long &offset);

        static std::size_t CalculatedSize(const PBEncodeItem &encodeItem);

        static std::size_t CalculatedSize(const std::size_t dataLen);

        static long PBEncodeItemHeadLen();

        static bool VerifyItem(const PBEncodeItem &encodeItem);

        PBEncodeItem DecodeData(const Buffer &buffer, int32_t &decode_len);

        bool DecodeData(PBEncodeItem &out, FILE *fp, long &offset);

    private:
        MiniPBCoder();

        ~MiniPBCoder();

        Buffer EncodeData(const Buffer &buffer, int cryptoFlag, const std::string &date);

        void WriteItem(Buffer &buffer, const PBEncodeItem &encodeItem);

    };
}
#endif //MINIPBCODER_H
