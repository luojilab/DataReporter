//
// Created by bingjian on 2018/11/5.
//
#include "File.h"
#include "AndroidUtil.h"

namespace future {

    bool File::MkPath(const std::string &path) {

        JNIEnv *env = AndroidUtil::getEnv();
        jstring javaPath = AndroidUtil::createJavaString(env, path);
        jobject javaFile = AndroidUtil::Constructor_File->call(javaPath);
        bool ret = AndroidUtil::Method_mkdirs->call(javaFile);
        env->DeleteLocalRef(javaPath);
        env->DeleteLocalRef(javaFile);
        return ret;
    }

    bool File::IsFileExist(const std::string &path) {

        JNIEnv *env = AndroidUtil::getEnv();
        jstring javaPath = AndroidUtil::createJavaString(env, path);
        jobject javaFile = AndroidUtil::Constructor_File->call(javaPath);
        bool ret = AndroidUtil::Method_exists->call(javaFile);
        env->DeleteLocalRef(javaPath);
        env->DeleteLocalRef(javaFile);
        return ret;
    }

    bool File::ReName(const std::string &oldPath, const std::string &newPath) {

        JNIEnv *env = AndroidUtil::getEnv();
        jstring javaOldPath = AndroidUtil::createJavaString(env, oldPath);
        jobject javaOldFile = AndroidUtil::Constructor_File->call(javaOldPath);
        jstring javaNewPath = AndroidUtil::createJavaString(env, newPath);
        jobject javaNewFile = AndroidUtil::Constructor_File->call(javaNewPath);
        bool ret = AndroidUtil::Method_rename->call(javaOldFile, javaNewFile);
        env->DeleteLocalRef(javaOldPath);
        env->DeleteLocalRef(javaOldFile);
        env->DeleteLocalRef(javaNewPath);
        env->DeleteLocalRef(javaNewFile);
        return ret;
    }

    bool File::RemoveFile(const std::string &path) {

        JNIEnv *env = AndroidUtil::getEnv();
        jstring javaPath = AndroidUtil::createJavaString(env, path);
        jobject javaFile = AndroidUtil::Constructor_File->call(javaPath);
        bool ret = AndroidUtil::Method_delete->call(javaFile);
        env->DeleteLocalRef(javaPath);
        env->DeleteLocalRef(javaFile);
        return ret;
    }

    std::shared_ptr<std::list<std::string> > File::FileList(const std::string &path) {
        JNIEnv *env = AndroidUtil::getEnv();
        std::shared_ptr<std::list<std::string> > retList = std::make_shared<std::list<std::string> >();
        jstring javaPath = AndroidUtil::createJavaString(env, path);
        jobject javaFile = AndroidUtil::Constructor_File->call(javaPath);
        jobjectArray files = AndroidUtil::Method_filelist->call(javaFile);
        if (files == NULL) {
            env->DeleteLocalRef(javaPath);
            env->DeleteLocalRef(javaFile);
            return retList;
        }
        jsize len = env->GetArrayLength(files);

        for (int i = 0; i < len; i++) {
            jobject jobj = env->GetObjectArrayElement(files, i);
            std::string tmpStr = AndroidUtil::fromJavaString(env, (jstring) jobj);
            retList->push_back(tmpStr);
            env->DeleteLocalRef(jobj);
        }

        env->DeleteLocalRef(files);
        env->DeleteLocalRef(javaPath);
        env->DeleteLocalRef(javaFile);

        return retList;
    }
}