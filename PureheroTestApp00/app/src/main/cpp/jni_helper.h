//
// Created by purehero on 2022-06-03.
//

#ifndef MYNDKAPP00_JNI_HELPER_H
#define MYNDKAPP00_JNI_HELPER_H

#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define ENABLE_LOG

#ifdef ENABLE_LOG
#define LOG_TAG "MyApp00"
#define __FILENAME__ (strstr(__FILE__, "/cpp/") ? strstr(__FILE__, "/cpp/") + 5 : __FILE__)
#define  LOGT()		__android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,"[LOGT] %s:%d -> %s",__FILENAME__,__LINE__,__func__)
#define  LOGV(...)  __android_log_print(ANDROID_LOG_VERBOSE,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#else
#define  LOGT()
    #define  LOGV(...)
    #define  LOGD(...)
    #define  LOGI(...)
    #define  LOGE(...)
    #define  LOGW(...)
    #define  LOGT()
#endif  // #ifdef ENABLE_LOG


#endif //MYNDKAPP00_JNI_HELPER_H
