//
// Created by purehero on 2024-02-06.
//
#include "utils/util.h"
#include "jni_helper.h"

#include "utils/Inotify.h"

void JNI_InitModule(JNIEnv* env, jobject obj);
void JNI_SetAlram(JNIEnv *env, jobject obj, jint seconds );
void JNI_OnClickSnackbar(JNIEnv* env, jobject obj, jobject view);

Inotify inotify;
void InofityEventALL( struct inotify_event * event );

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((constructor))
int JNI_OnPreLoad() {
    LOGT();
    return 0;
}

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    LOGT();

    JNIEnv *env = NULL;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        LOGE("ERROR: GetEnv failed");
        return -1;
    }

    const char *strNativeClassname = OBFUSCATED("com/purehero/app00/NativeModule00");

    jclass clazz = env->FindClass(strNativeClassname);
    if (clazz == NULL) {
        LOGE("ERROR: FineClass => %s", strNativeClassname);
        return JNI_FALSE;
    }
    JNINativeMethod native_methods[] = {
            {OBFUSCATED("init_module"),     OBFUSCATED("()V"),                    (void *) JNI_InitModule},
            {OBFUSCATED("setAlram"),     OBFUSCATED("(I)V"),                    (void *) JNI_SetAlram},
            {OBFUSCATED("onClickSnackbar"), OBFUSCATED(
                                                    "(Landroid/view/View;)V"),    (void *) JNI_OnClickSnackbar},
    };

    if (env->RegisterNatives(clazz, native_methods,
                             sizeof(native_methods) / sizeof(native_methods[0])) < 0) {
        LOGE("ERROR: RegisterNatives => %s %d method", strNativeClassname,
             (int) (sizeof(native_methods) / sizeof(native_methods[0])));
        return JNI_FALSE;
    }

#if defined(__i386__)
    LOGD(">>> X86 MODULE <<<");
#elif defined(__x86_64__)    // arm 64bit
    LOGD(">>> X64 MODULE <<<");
#elif defined(__aarch64__)	// arm 64bit
    LOGD(">>> ARM64 MODULE <<<");
#else
    LOGD(">>> ARM32 MODULE <<<");
#endif

    inotify.Init(OBFUSCATED("/proc/self"));
    inotify.RegisterCallback(InofityEventALL);
    inotify.Start();

    return JNI_VERSION_1_6;
}

#ifdef __cplusplus
}
#endif


void JNI_SetAlram(JNIEnv *env, jobject obj, jint seconds ) {
    LOGT();
    signal( SIGALRM, SIG_DFL );
    alarm( seconds );
}

void JNI_InitModule(JNIEnv *env, jobject obj) {
    LOGT();

    void *p_fopen = (void *) fopen;
    LOGD("fopen addr : 0x%p", p_fopen);

}

void JNI_OnClickSnackbar(JNIEnv *env, jobject obj, jobject view) {
    LOGT();
}

void InofityEventALL(struct inotify_event *event) {
    if (event->mask & IN_OPEN) {
        LOGD("===> [O]%s", event->name);

    } else if(event->mask & IN_CLOSE_WRITE || event->mask & IN_CLOSE_NOWRITE) {
        LOGD("===> [C]%s", event->name);
    }
}
