//
// Created by purehero on 2024-01-11.
//

#include "CheckJumpCode.h"

void CheckJumpCode::init() {
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