//
// Created by purehero on 2024-01-11.
//

#include "CheckJumpCode.h"
#include "jni_helper.h"

CheckJumpCode * CheckJumpCode::CreateInstanceAllMethos()
{
    CheckJumpCode * inst = new CheckJumpCode();
    inst->AddAllMethod();
    return inst;
}

void CheckJumpCode::AddAllMethod() {
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

FUNCTION_INFO * CheckJumpCode::CheckAllMethod()
{
    int func_list_count = func_infos.size();
    for (int i(0) ; i < func_list_count; ++i) {
        if (CheckMethod((unsigned char *) func_infos[i].func_ptr)) {
            return &func_infos[i];
        }
    }

    return NULL;
}

//__attribute__((always_inline))
bool CheckJumpCode::CheckMethod( unsigned char* ptr )
{
#if defined(__i386__) || defined(__x86_64__)
    if (*ptr == 0xE9) {
        return true;					// https://tmdahr1245.tistory.com/103
    }

#elif defined(__aarch64__)	// arm 64bit
    /* 	50 00 00 58 :  		ldr x16, #8
		00 02 1f d6 : 		br x16
					or
		20 02 1f d6 : 		br x17
	 */
	if ((*(ptr+4) == 0x00 || *(ptr+4) == 0x20) && *(ptr+5) == 0x02 && *(ptr+6) == 0x1F && *(ptr+7) == 0xD6) {
		return true;
	}

#else	// arm 32bit
    if (*ptr == 0x04 && *(ptr+1) == 0xF0 && *(ptr+2) == 0x1F && *(ptr+3) == 0xE5) { // arm
        // 04 f0 1f e5 : ldr pc, [pc, #-4]
        return true;
    }

    if (*ptr == 0xF8 && (*(ptr+1)&0xF0) == 0 && *(ptr+2) == 0xF0 && (*(ptr+3)&0xF0) == 0) { // arm thumb32
        // f8 0{x1} f0 0{x2} : strb.w pc, [r{x1}, r{x2}]
        return true;
    }

    if (*ptr == 0xB5 && (*(ptr+1)&0xF0) == 0 && (*(ptr+2)&0xF0) == 0xF0) { // arm thumb16
        // b5 0{x1} f{x2} {x3}{x4} : strlt pc, [r{x1}, #-0x{x2}{x3}{x4}]
        return true;
    }
#endif
    return false;
}
