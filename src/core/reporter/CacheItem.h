//
// Created by bingjian on 2018/11/8.
//

#ifndef DATA_REPORTER_CACHEITEM_H
#define DATA_REPORTER_CACHEITEM_H

#include "PBEncodeItem.hpp"

namespace future {

    struct CacheItem {
        PBEncodeItem pbEncodeItem;
        std::string fromPath = "";
        void *fromMem = NULL;
    };
}

#endif //DATA_REPORTER_CACHEITEM_H
