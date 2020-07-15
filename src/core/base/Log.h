#ifndef LOG_H
#define LOG_H

#include "Platform.h"

#if PLATFORM(ANDROID)

#include <android/log.h>

#endif

#include <cstring>
#include <errno.h>

namespace future {
#if PLATFORM(ANDROID)
// enable logging
  #define ENABLE_LOG

    #ifdef ENABLE_LOG

    #define LOG_TAG "DataReporter"

    #define Error(format, ...)                                                                     \
    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, format, ##__VA_ARGS__)
    #define Warning(format, ...)                                                                   \
    __android_log_print(ANDROID_LOG_WARN, LOG_TAG, format, ##__VA_ARGS__)
    #define Info(format, ...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, format, ##__VA_ARGS__)

      #ifndef NDEBUG
      #define Debug(format, ...)                                                                     \
      __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, format, ##__VA_ARGS__)
      #else
      #define Debug(format, ...)                                                                     \
      {}
      #endif

     #else

     #endif

#elif PLATFORM(IOS)
    #define Error(format, ...)                                                                     \
    printf(format, ##__VA_ARGS__)
    #define Warning(format, ...)                                                                   \
    printf(format, ##__VA_ARGS__)
    #define Info(format, ...)                                                                      \
    printf(format, ##__VA_ARGS__)
      #ifndef NDEBUG
        #define Debug(format, ...)                                                                     \
        printf(format, ##__VA_ARGS__)
      #else
        #define Debug(format, ...)                                                                     \
        {}
      #endif
#else
#define Error(format, ...)                                                                     \
    {}
#define Warning(format, ...)                                                                   \
    {}
#define Info(format, ...)                                                                      \
    {}
#define Debug(format, ...)                                                                     \
    {}
#endif

    inline void print_hex(unsigned char *buf, uint64_t len) {
        //Print results:
        int i;
        for (i = 0; i < len; i++) {
            Debug("%02X", buf[i]);
            if (15 == i % 16){
                Debug("\n");
            }
        }
        Debug("\n");
    }

    inline void byte_to_hex_str(const unsigned char* in, int inLen, char* out )
    {
        short i;
        unsigned char highByte, lowByte;

        for (i = 0; i < inLen; i++)
        {
            highByte = in[i] >> 4;
            lowByte = in[i] & 0x0f ;

            highByte += 0x30;

            if (highByte > 0x39){
                out[i * 2] = highByte + 0x07;
            }else{
                out[i * 2] = highByte;
            }
            lowByte += 0x30;
            if (lowByte > 0x39){
                out[i * 2 + 1] = lowByte + 0x07;
            }else{
                out[i * 2 + 1] = lowByte;
            }
        }
        return ;
    }

    inline void print_matrix(unsigned char *buf, uint64_t len) { // 打印16列的矩阵
        int i;
        int j = 0;
        unsigned char inBuf[16];
        char hexBuf[32 + 1];
        for (i = 0; i < len; i++) {
            inBuf[j++] = buf[i];
            if (15 == i % 16){
                j = 0;
                byte_to_hex_str(inBuf, sizeof(inBuf),hexBuf);
                hexBuf[32] = 0;
                Debug("%s\n",hexBuf);
            }
        }

        if(j != 0){
            byte_to_hex_str(inBuf, j, hexBuf);
            hexBuf[j + 1] = 0;
            Debug("%s\n",hexBuf);
        }
    }

}
#endif //LOG_H
