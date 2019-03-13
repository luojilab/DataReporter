//
// Created by bingjian on 2018/11/1.
//

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <StringUtil.h>
#include "File.h"
#include "Log.h"

namespace future {

    bool File::ZeroFillFile(int fd, size_t startPos, size_t size) {
        if (fd < 0) {
            return false;
        }

        if (lseek(fd, startPos, SEEK_SET) < 0) {
            Error("fail to lseek fd[%d], error:%s", fd, strerror(errno));
            return false;
        }

        const char zeros[4096] = {0};
        while (size >= sizeof(zeros)) {
            if (write(fd, zeros, sizeof(zeros)) < 0) {
                Error("fail to write fd[%d], error:%s", fd, strerror(errno));
                return false;
            }
            size -= sizeof(zeros);
        }

        if (size > 0 && (write(fd, zeros, size) < 0)) {
            Error("fail to write fd[%d], error:%s", fd, strerror(errno));
            return false;
        }
        return true;
    }

    std::string File::GetFileName(const std::string &path) {
        std::size_t pos = path.find_last_of("/");
        if (pos == std::string::npos) {
            return path.substr(0, path.find_last_of("."));
        } else {
            std::string tmp = path.substr(pos + 1);
            return tmp.substr(0, tmp.find_last_of("."));
        }
    }

    std::string File::GetFileNameWithExt(const std::string &path) {
        std::size_t pos = path.find_last_of("/");
        if (pos == std::string::npos) {
            return path;
        } else {
            std::string ret = path.substr(pos + 1);
            return std::move(ret);
        }
    }

    std::list<std::string>
    File::FilterByFun(const std::list<std::string> &files,
                      std::function<bool(const std::string &fileName)> fun) {
        std::list<std::string> ret;
        for (std::list<std::string>::const_iterator iter = files.begin();
             iter != files.end(); iter++) {
            if (fun(*iter)) {
                ret.push_back(*iter);
            }
        }
        return std::move(ret);
    }

}