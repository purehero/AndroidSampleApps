#include "util.h"
#include "jni_helper.h"

#include "MapsFile.h"

void JNI_InitModule(JNIEnv* env, jobject obj) {
    LOGT();

    void * p_fopen = (void *) fopen;
    LOGD( "fopen addr : 0x%p", p_fopen );

    std::string result;
    bytesToHexString((unsigned char *) p_fopen, 32, result );
    LOGD( "%s", result.c_str());
}

#include <dlfcn.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/mman.h>
#include <sys/ptrace.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/inotify.h>

#include <vector>
std::vector<std::string> g_func_info_hex_strings;

#define DEF_FUNC_INFO(x)    {#x,(size_t)x}
#define DEF_FUNC_INFO2(x)    {"",(size_t)x}
struct func_info {
    const char * func_name;
    size_t func_ptr;
};

std::vector<struct func_info> func_infos;
void InitJumpCodeCheckMethods() {
    LOGT();

    func_infos.push_back(DEF_FUNC_INFO(_exit));
    func_infos.push_back(DEF_FUNC_INFO(access));
    func_infos.push_back(DEF_FUNC_INFO(alarm));
    func_infos.push_back(DEF_FUNC_INFO(clock_gettime));
    func_infos.push_back(DEF_FUNC_INFO(dlopen));            // 0 ~ 4

    func_infos.push_back(DEF_FUNC_INFO(exit));
    func_infos.push_back(DEF_FUNC_INFO(fopen));
    func_infos.push_back(DEF_FUNC_INFO(fork));
    func_infos.push_back(DEF_FUNC_INFO(inotify_add_watch));
    func_infos.push_back(DEF_FUNC_INFO(inotify_rm_watch));  // 5 ~ 9

    func_infos.push_back(DEF_FUNC_INFO(kill));
    func_infos.push_back(DEF_FUNC_INFO(memcmp));
    func_infos.push_back(DEF_FUNC_INFO(memcpy));
    func_infos.push_back(DEF_FUNC_INFO(mprotect));
    //func_infos.push_back(DEF_FUNC_INFO(open));              // 10 ~ 14

    func_infos.push_back(DEF_FUNC_INFO(opendir));
    func_infos.push_back(DEF_FUNC_INFO(prctl));
    func_infos.push_back(DEF_FUNC_INFO(pthread_create));
    func_infos.push_back(DEF_FUNC_INFO(ptrace));
    func_infos.push_back(DEF_FUNC_INFO(raise));             // 15 ~ 19

    func_infos.push_back(DEF_FUNC_INFO(readdir));
    func_infos.push_back(DEF_FUNC_INFO(sigaction));
    func_infos.push_back(DEF_FUNC_INFO(snprintf));
    func_infos.push_back(DEF_FUNC_INFO(sprintf));
    func_infos.push_back(DEF_FUNC_INFO(strcmp));            // 20 ~ 24

    func_infos.push_back(DEF_FUNC_INFO(strcpy));
    func_infos.push_back(DEF_FUNC_INFO(strncmp));
    func_infos.push_back(DEF_FUNC_INFO(strncpy));
    //func_infos.push_back(DEF_FUNC_INFO(strstr));
    func_infos.push_back(DEF_FUNC_INFO(atol));              // 25 ~ 29

    func_infos.push_back(DEF_FUNC_INFO(fread));
    func_infos.push_back(DEF_FUNC_INFO(getpgid));
    func_infos.push_back(DEF_FUNC_INFO(getpid));
    func_infos.push_back(DEF_FUNC_INFO(getppid));
    func_infos.push_back(DEF_FUNC_INFO(gettid));            // 30 ~ 34

    func_infos.push_back(DEF_FUNC_INFO(memset));
    func_infos.push_back(DEF_FUNC_INFO(mmap));
    func_infos.push_back(DEF_FUNC_INFO(munmap));
    func_infos.push_back(DEF_FUNC_INFO(signal));
    func_infos.push_back(DEF_FUNC_INFO(sscanf));            // 35 ~ 39

    func_infos.push_back(DEF_FUNC_INFO(waitpid));
    func_infos.push_back(DEF_FUNC_INFO(chmod));
    func_infos.push_back(DEF_FUNC_INFO(remove));
    func_infos.push_back(DEF_FUNC_INFO(system));
    func_infos.push_back(DEF_FUNC_INFO(dlsym));             // 40 ~ 44
}
// _exit, exit : frida 가 로딩되면 기본으로 hooking 됨

void JNI_OnClickSnackbar(JNIEnv* env, jobject obj, jobject view) {
    LOGT();

    bool bFirst = g_func_info_hex_strings.empty();

    int func_list_count = func_infos.size();
    for (int i(0) ; i < func_list_count; ++i) {
        std::string result;
        bytesToHexString((unsigned char *) func_infos[i].func_ptr, 16, result );

        if (bFirst) {
            g_func_info_hex_strings.push_back(result);
            LOGD( "[%2d] %s : %s", i, result.c_str(), func_infos[i].func_name);

        } else {
            if (result.compare( g_func_info_hex_strings[i]) == 0) {
                LOGD( "[%2d] %s : %s", i, result.c_str(), func_infos[i].func_name);
            } else {
                LOGE( "[%2d] %s => %s : %s", i, g_func_info_hex_strings[i].c_str(), result.c_str(), func_infos[i].func_name);
            }
        }
    }

    parse_maps( MapsDataCallbackMethod );
}

bool g_thread_check_native_api_hook = true;
void* thread_check_native_api_hook( void* pEnv)
{
    LOGT();
    JNIEnv* env = (JNIEnv*)pEnv;

    int func_list_count = func_infos.size();
    while (g_thread_check_native_api_hook) {
        for (int i(0) ; i < func_list_count; ++i) {
            unsigned char *ptr = (unsigned char *)func_infos[i].func_ptr;
            bool isJumpCode = false;

#if defined(__i386__) || defined(__x86_64__)
            if (*ptr == 0xE9) {						// https://tmdahr1245.tistory.com/103
                isJumpCode = true;
            }
#elif defined(__aarch64__)	// arm 64bit
            /* 	50 00 00 58 :  		ldr x16, #8
                00 02 1f d6 : 		br x16
                            or
                20 02 1f d6 : 		br x17
             */
            if ((*(ptr+4) == 0x00 || *(ptr+4) == 0x20) && *(ptr+5) == 0x02 && *(ptr+6) == 0x1F && *(ptr+7) == 0xD6) {
                isJumpCode = true;
            }
#else	// arm 32bit
            if (*ptr == 0x04 && *(ptr+1) == 0xF0 && *(ptr+2) == 0x1F && *(ptr+3) == 0xE5) { // arm					// 04 f0 1f e5 : ldr pc, [pc, #-4]
                isJumpCode = true;

            } else if (*ptr == 0xF8 && (*(ptr+1)&0xF0) == 0 && *(ptr+2) == 0xF0 && (*(ptr+3)&0xF0) == 0) { // arm thumb32		// f8 0{x1} f0 0{x2} : strb.w pc, [r{x1}, r{x2}]
                isJumpCode = true;

            } else if (*ptr == 0xB5 && (*(ptr+1)&0xF0) == 0 && (*(ptr+2)&0xF0) == 0xF0) { // arm thumb16						// b5 0{x1} f{x2} {x3}{x4} : strlt pc, [r{x1}, #-0x{x2}{x3}{x4}]
                isJumpCode = true;
            }
#endif
            if (isJumpCode) {
                std::string result;
                bytesToHexString((unsigned char *) func_infos[i].func_ptr, 16, result );

                LOGE( "Found hook api : %s [%s]", func_infos[i].func_name, result.c_str());
            }

            usleep( 10 );
        }

        usleep( 100 );
    }

    return NULL;
}

#include <sys/types.h>
#include <sys/inotify.h>

#define EVENT_SIZE      ( sizeof (struct inotify_event) )
#define BUF_LEN         ( 1024 * ( EVENT_SIZE + 16 ) )

void * thread_maps_inotify(void *) {
    LOGT();

    int inotify_fd = inotify_init();
    int wd = inotify_add_watch( inotify_fd, (const char*)"/proc/self/maps", IN_ALL_EVENTS );

    char buffer[BUF_LEN];
    while( true ) {
        int len = read(inotify_fd, buffer, BUF_LEN);
        for (int i = 0; i < len; i += EVENT_SIZE) {
            struct inotify_event *event = (struct inotify_event *) &buffer[i];

            if (event->mask & IN_CREATE) {
                if (event->mask & IN_ISDIR) {
                    LOGD("The directory %s was created.", event->name);
                } else {
                    LOGD("The file %s was created.", event->name);
                }
            } else if (event->mask & IN_DELETE) {
                if (event->mask & IN_ISDIR) {
                    LOGD("The directory %s was deleted.", event->name);
                } else {
                    LOGD("The file %s was deleted.", event->name);
                }
            } else if (event->mask & IN_MODIFY) {
                if (event->mask & IN_ISDIR) {
                    LOGD("The directory %s was modified.", event->name);
                } else {
                    LOGD("The file %s was modified.", event->name);
                }
            } else if (event->mask & IN_MOVED_FROM || event->mask & IN_MOVED_TO || event->mask & IN_MOVE_SELF) {
                if (event->mask & IN_ISDIR) {
                    LOGD("The directory %s was moved.", event->name);
                } else {
                    LOGD("The file %s was moved.", event->name);
                }
            }

            i += event->len;
        }
    }

    inotify_rm_watch(inotify_fd, wd);
    close(inotify_fd);

    return NULL;
}

void JNI_OnUnload(JavaVM* vm, void* reserved)
{
    LOGT();
    g_thread_check_native_api_hook = false;
}

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    LOGT();
    LOGD( "PID:%d, PPID:%d", getpid(), getppid());

    JNIEnv* env = NULL;
    if(vm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK) {
        LOGE("ERROR: GetEnv failed");
        return -1;
    }

    const char * strNativeClassname = "com/purehero/app00/NativeModule00";
    jclass clazz = env->FindClass( strNativeClassname );
    if( clazz == NULL ) {
        LOGE( "ERROR: FineClass => %s", strNativeClassname );
        return JNI_FALSE;
    }
    JNINativeMethod native_methods [] = {
            { "init_module", "()V", (void*) JNI_InitModule },
            { "onClickSnackbar", "(Landroid/view/View;)V", (void*) JNI_OnClickSnackbar },
    };

    if( env->RegisterNatives( clazz, native_methods, sizeof(native_methods)/sizeof(native_methods[0])) < 0 ) {
        LOGE( "ERROR: RegisterNatives => %s %d method", strNativeClassname, (int)(sizeof(native_methods)/sizeof(native_methods[0])));
        return JNI_FALSE;
    }

#if defined(__i386__)
    LOGD("[X86 MODULE]");
#elif defined(__x86_64__)	// arm 64bit
        LOGD("[X64 MODULE]");
#elif defined(__aarch64__)	// arm 64bit
    LOGD("[ARM64 MODULE]");
#else
    LOGD("[ARM32 MODULE]");
#endif
    InitJumpCodeCheckMethods();

    struct func_info new_func;
    new_func.func_name = "";
    new_func.func_ptr = (size_t) env->functions->FindClass;
    func_infos.push_back(new_func);

    pthread_t tid;
    pthread_create( &tid, NULL, thread_check_native_api_hook, env );
    pthread_create( &tid, NULL, thread_maps_inotify, env );

    return JNI_VERSION_1_6;
}
