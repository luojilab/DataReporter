//
// Created by bingjian on 2018/11/7.
//

#include "IoUtil.h"

namespace future {

    bool IoUtil::WriteFile(const std::string &path, void *safeBuf, long count) {
        FILE *fp = fopen(path.c_str(), "wb+");
        if (fp == NULL) {
            return false;
        }

        fwrite(safeBuf, 1, count, fp);
        fclose(fp);
        return true;
    }
}