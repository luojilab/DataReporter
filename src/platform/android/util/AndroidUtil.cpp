#include <stddef.h>
#include <jni.h>
#include "AndroidUtil.h"

JavaVM *AndroidUtil::JAVA_VM = NULL;

JavaClass AndroidUtil::Class_java_lang_Object("java/lang/Object");
JavaClass AndroidUtil::CLASS_java_File("java/io/File");
JavaClass AndroidUtil::CLASS_java_String("java/lang/String");
JavaClass AndroidUtil::CLASS_java_DataReporter("com/iget/datareporter/DataReporter");

std::shared_ptr<Constructor> AndroidUtil::Constructor_File;
std::shared_ptr<BooleanMethod> AndroidUtil::Method_mkdir;
std::shared_ptr<BooleanMethod> AndroidUtil::Method_mkdirs;
std::shared_ptr<BooleanMethod> AndroidUtil::Method_isDirectory;
std::shared_ptr<BooleanMethod> AndroidUtil::Method_exists;
std::shared_ptr<BooleanMethod> AndroidUtil::Method_delete;
std::shared_ptr<BooleanMethod> AndroidUtil::Method_rename;
std::shared_ptr<ObjectArrayMethod> AndroidUtil::Method_filelist;
std::shared_ptr<StaticVoidMethod> AndroidUtil::Method_upload;

JNIEnv *AndroidUtil::getEnv() {
    JNIEnv *env;
    JAVA_VM->GetEnv((void **) &env, JNI_VERSION_1_2);
    return env;
}

JNIEnv *AndroidUtil::attachCurrentThread() {
    JNIEnv *env;
    JAVA_VM->AttachCurrentThread(&env, NULL);
    return env;
}

void AndroidUtil::detachCurrentThread() {
    JAVA_VM->DetachCurrentThread();
}

bool AndroidUtil::init(JavaVM *jvm) {
    JAVA_VM = jvm;
    CLASS_java_String.j();
    CLASS_java_DataReporter.j();

    Constructor_File = std::shared_ptr<Constructor>(
            new Constructor(AndroidUtil::CLASS_java_File, "(Ljava/lang/String;)V"));
    Method_mkdir = std::shared_ptr<BooleanMethod>(
            new BooleanMethod(AndroidUtil::CLASS_java_File, "mkdir", "()"));
    Method_mkdirs = std::shared_ptr<BooleanMethod>(
            new BooleanMethod(AndroidUtil::CLASS_java_File, "mkdirs", "()"));
    Method_isDirectory = std::shared_ptr<BooleanMethod>(
            new BooleanMethod(AndroidUtil::CLASS_java_File, "isDirectory", "()"));
    Method_exists = std::shared_ptr<BooleanMethod>(
            new BooleanMethod(AndroidUtil::CLASS_java_File, "exists", "()"));
    Method_delete = std::shared_ptr<BooleanMethod>(
            new BooleanMethod(AndroidUtil::CLASS_java_File, "delete", "()"));
    Method_rename = std::shared_ptr<BooleanMethod>(
            new BooleanMethod(AndroidUtil::CLASS_java_File, "renameTo", "(Ljava/io/File;)"));
    JavaArray stringArray(CLASS_java_String);
    Method_filelist = std::shared_ptr<ObjectArrayMethod>(
            new ObjectArrayMethod(AndroidUtil::CLASS_java_File, "list", stringArray, "()"));
    Method_upload = std::shared_ptr<StaticVoidMethod>(
            new StaticVoidMethod(AndroidUtil::CLASS_java_DataReporter, "upload",
                                 "(J[Ljava/lang/String;Lcom/iget/datareporter/IReport;)"));
    return true;
}

JObject::JObject(jobject object) {
    m_Object = AndroidUtil::getEnv()->NewGlobalRef(object);
}

JObject::~JObject() {
    AndroidUtil::getEnv()->DeleteGlobalRef(m_Object);
}

JString::JString(JNIEnv *env, const std::string &str, bool emptyIsNull) : m_env(env) {
    m_J = (emptyIsNull && str.empty()) ? NULL : env->NewStringUTF(str.c_str());
}

JString::~JString() {
    if (m_J != NULL) {
        m_env->DeleteLocalRef(m_J);
    }
}

jstring AndroidUtil::createJavaString(JNIEnv *env, const std::string &str) {
    if (str.empty()) {
        return env->NewStringUTF("");
    }
    return env->NewStringUTF(str.c_str());
}

std::string AndroidUtil::convertNonUtfString(const std::string &str) {
    const int len = str.length();
    if (len == 0) {
        return str;
    }

    JNIEnv *env = getEnv();

    jchar *chars = new jchar[len];
    for (int i = 0; i < len; ++i) {
        chars[i] = (unsigned char) str[i];
    }
    jstring javaString = env->NewString(chars, len);
    const std::string result = fromJavaString(env, javaString);
    env->DeleteLocalRef(javaString);
    delete[] chars;

    return result;
}

std::string AndroidUtil::fromJavaString(JNIEnv *env, jstring from) {
    if (from == NULL) {
        return std::string("");
    }
    const char *data = env->GetStringUTFChars(from, 0);
    const std::string result(data);
    env->ReleaseStringUTFChars(from, data);
    return result;
}

jintArray AndroidUtil::createJavaIntArray(JNIEnv *env, const std::vector<jint> &data) {
    std::size_t size = data.size();
    jintArray array = env->NewIntArray(size);
    env->SetIntArrayRegion(array, 0, size, &data.front());
    return array;
}

jbyteArray AndroidUtil::createJavaByteArray(JNIEnv *env, const std::vector<jbyte> &data) {
    std::size_t size = data.size();
    jbyteArray array = env->NewByteArray(size);
    env->SetByteArrayRegion(array, 0, size, &data.front());
    return array;
}

