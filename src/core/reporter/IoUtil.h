//
// Created by bingjian on 2018/11/7.
//

#ifndef DATA_REPORTER_IOUTIL_H
#define DATA_REPORTER_IOUTIL_H

#include <string>

namespace future {
    class IoUtil {
    public:
        static bool WriteFile(const std::string &path, void *safeBuf , long count);
    };
}

#endif //DATA_REPORTER_IOUTIL_H
