#import <Foundation/Foundation.h>
#include "File.h"

namespace future {

    bool File::MkPath(const std::string &path) {

        NSString *filePathAndDirectory = [NSString stringWithUTF8String:path.c_str()];
        NSError *error = nil;
        
        [[NSFileManager defaultManager] createDirectoryAtPath:filePathAndDirectory
                                  withIntermediateDirectories:NO
                                                   attributes:nil
                                                        error:&error];
        return error == nil ? true : false;
    }

    bool File::IsFileExist(const std::string &path) {
        NSString *filePath = [NSString stringWithUTF8String:path.c_str()];
        return [[NSFileManager defaultManager] fileExistsAtPath:filePath];
    }

    bool File::ReName(const std::string &oldPath, const std::string &newPath) {
        NSError *error = nil;
        [[NSFileManager defaultManager] moveItemAtPath:[NSString stringWithUTF8String:oldPath.c_str()]
                                                toPath:[NSString stringWithUTF8String:newPath.c_str()]
                                                 error:&error];
        return error == nil ? true : false;
    }

    bool File::RemoveFile(const std::string &path) {
        NSError *error;
        [[NSFileManager defaultManager] removeItemAtPath:[NSString stringWithUTF8String:path.c_str()] error:&error];
        
        if (error) {
            return false;
        } else {
            return true;
        }
    }

    std::shared_ptr<std::list<std::string> > File::FileList(const std::string &path) {
        NSError * error;
        NSArray * directoryContents = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:[NSString stringWithUTF8String:path.c_str()] error:&error];

        std::shared_ptr<std::list<std::string> > retList = std::make_shared<std::list<std::string> >();
        if (!error) {
            for (NSString *path in directoryContents) {
                std::string str = [path UTF8String];
                retList->push_back(str);
            }
        }
        
        return retList;
    }
}
