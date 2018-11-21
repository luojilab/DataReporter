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
        static Buffer EncodeString(const std::string &str) {
            MiniPBCoder pbcoder;
            return pbcoder.EncodeData(str);
        }

        static PBEncodeItem DecodeItem(const Buffer &buffer);

        static bool DecodeItem(PBEncodeItem &out, FILE *fp, long &offset);

        static std::size_t CalculatedSize(const PBEncodeItem &encodeItem);

        static std::size_t CalculatedSize(const std::string &data);

        static long PBEncodeItemHeadLen();

        static bool VerifyItem(const PBEncodeItem &encodeItem);

        PBEncodeItem DecodeData(const Buffer &buffer);

        bool DecodeData(PBEncodeItem &out, FILE *fp, long &offset);

    private:
        MiniPBCoder();

        ~MiniPBCoder();

        Buffer EncodeData(const std::string &str);

        Buffer EncodeData(const Buffer &buffer);

        void WriteItem(Buffer &buffer, const PBEncodeItem &encodeItem);

    };
}
#endif //MINIPBCODER_H
