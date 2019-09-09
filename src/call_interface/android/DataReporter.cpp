#include <jni.h>
#include <string>
#include "JniHelper.h"
#include "AndroidUtil.h"
#include "Reporter.h"

static jlong
MakeReporter(JNIEnv *env, jobject obj, jstring uuid, jstring cachePath, jstring encryptKey,
             jobject reportImp) {
    std::string uuidCStr = AndroidUtil::fromJavaString(env, uuid);
    std::string cachePathCstr = AndroidUtil::fromJavaString(env, cachePath);
    std::string encryptKeyCstr = AndroidUtil::fromJavaString(env, encryptKey);
    std::shared_ptr<JObject> jCallback(new JObject(reportImp));
    future::Reporter *reporter = new future::Reporter(uuidCStr, cachePathCstr, encryptKeyCstr,
                                                      [jCallback](int64_t key,
                                                                  std::list<std::shared_ptr<future::CacheItem> > &data) {
                                                          JNIEnv *env = AndroidUtil::getEnv();
                                                          jobjectArray javaData = env->NewObjectArray(
                                                                  data.size(),
                                                                  AndroidUtil::CLASS_java_ByteArray.j(),
                                                                  NULL);
                                                          int i = 0;
                                                          for (std::list<std::shared_ptr<future::CacheItem> >::iterator iter = data.begin();
                                                               iter != data.end(); iter++) {
                                                              if((*iter)->pbEncodeItem.data.Length() == 0){
                                                                  continue;
                                                              }

                                                              jbyteArray itemByteArray = env->NewByteArray((*iter)->pbEncodeItem.data.Length());
                                                              env->SetByteArrayRegion(itemByteArray, 0, (*iter)->pbEncodeItem.data.Length(), (jbyte *)(*iter)->pbEncodeItem.data.GetBegin());
                                                              env->SetObjectArrayElement(
                                                                      javaData, i, itemByteArray);
                                                              i++;
                                                              env->DeleteLocalRef(itemByteArray);
                                                          }
                                                          AndroidUtil::Method_upload->call(
                                                                  (jlong) key, javaData,
                                                                  jCallback->GetObj());
                                                          env->DeleteLocalRef(javaData);
                                                      });
    return (jlong) reporter;
}

static void SetReportCount(JNIEnv *env, jobject obj, jlong nativeReporter, jint count) {
    future::Reporter *reporter = reinterpret_cast<future::Reporter *>(nativeReporter);
    if (reporter == NULL) {
        return;
    }
    reporter->SetUploadItemSize((int) count);
}

static void SetFileMaxSize(JNIEnv *env, jobject obj, jlong nativeReporter, jint fileMaxSize) {
    future::Reporter *reporter = reinterpret_cast<future::Reporter *>(nativeReporter);
    if (reporter == NULL) {
        return;
    }
    reporter->SetFileMaxSize((int) fileMaxSize);
}

static void SetExpiredTime(JNIEnv *env, jobject obj, jlong nativeReporter, jlong expiredTime) {
    future::Reporter *reporter = reinterpret_cast<future::Reporter *>(nativeReporter);
    if (reporter == NULL) {
        return;
    }
    reporter->SetExpiredTime((std::int64_t) expiredTime);
}

static void
SetReportingInterval(JNIEnv *env, jobject obj, jlong nativeReporter, jlong reportingInterval) {
    future::Reporter *reporter = reinterpret_cast<future::Reporter *>(nativeReporter);
    if (reporter == NULL) {
        return;
    }
    reporter->SetReportingInterval((std::int64_t) reportingInterval);
}

static void Start(JNIEnv *env, jobject obj, jlong nativeReporter) {
    future::Reporter *reporter = reinterpret_cast<future::Reporter *>(nativeReporter);
    if (reporter == NULL) {
        return;
    }
    reporter->Start();
}

static void ReaWaken(JNIEnv *env, jobject obj, jlong nativeReporter) {
    future::Reporter *reporter = reinterpret_cast<future::Reporter *>(nativeReporter);
    if (reporter == NULL) {
        return;
    }
    reporter->ReaWaken();
}

static void Push(JNIEnv *env, jobject obj, jlong nativeReporter, jbyteArray data) {
    future::Reporter *reporter = reinterpret_cast<future::Reporter *>(nativeReporter);
    if (reporter == NULL) {
        return;
    }

    std::size_t dataLen = env->GetArrayLength(data);
    std::vector<unsigned char> cData(dataLen);

    jbyte *dataJava = env->GetByteArrayElements(data, JNI_FALSE);
    for (int i = 0; i < dataLen; i++) {
        cData[i] = dataJava[i];
    }
    env->ReleaseByteArrayElements(data, dataJava, JNI_FALSE);

    reporter->Push(cData);
}

static void UploadSucess(JNIEnv *env, jobject obj, jlong nativeReporter, jlong key) {
    future::Reporter *reporter = reinterpret_cast<future::Reporter *>(nativeReporter);
    if (reporter == NULL) {
        return;
    }
    reporter->UoloadSuccess(key);
}

static void UploadFailed(JNIEnv *env, jobject obj, jlong nativeReporter, jlong key) {
    future::Reporter *reporter = reinterpret_cast<future::Reporter *>(nativeReporter);
    if (reporter == NULL) {
        return;
    }
    reporter->UploadFailed(key);
}

static void ReleaseReporter(JNIEnv *env, jobject obj, jlong nativeReporter) {
    future::Reporter *reporter = reinterpret_cast<future::Reporter *>(nativeReporter);
    if (reporter == NULL) {
        return;
    }
    future::Reporter::Destroy(reporter);
}

static JNINativeMethod gJavaDataReporterMethods[] = {
        {"makeReporter",         "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Lcom/iget/datareporter/IReport;)J", (void *) MakeReporter},
        {"setReportCount",       "(JI)V",                                                                                    (void *) SetReportCount},
        {"setFileMaxSize",       "(JI)V",                                                                                    (void *) SetFileMaxSize},
        {"setExpiredTime",       "(JJ)V",                                                                                    (void *) SetExpiredTime},
        {"setReportingInterval", "(JJ)V",                                                                                    (void *) SetReportingInterval},
        {"start",                "(J)V",                                                                                     (void *) Start},
        {"reaWaken",             "(J)V",                                                                                     (void *) ReaWaken},
        {"push",                 "(J[B)V",                                                                                   (void *) Push},
        {"uploadSucess",         "(JJ)V",                                                                                    (void *) UploadSucess},
        {"uploadFailed",         "(JJ)V",                                                                                    (void *) UploadFailed},
        {"releaseReporter",      "(J)V",                                                                                     (void *) ReleaseReporter},
};

int registerDataReporter(JNIEnv *env) {
    const char *reporterClassName = "com/iget/datareporter/DataReporter";
    return registerNativeMethods(env, reporterClassName,
                                 gJavaDataReporterMethods, NELEM(gJavaDataReporterMethods));
}
