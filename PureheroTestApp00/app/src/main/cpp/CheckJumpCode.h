//
// Created by purehero on 2024-01-11.
//

#ifndef PUREHEROTESTAPP00_CHECKJUMPCODE_H
#define PUREHEROTESTAPP00_CHECKJUMPCODE_H

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

#define DEF_FUNC_INFO(x)    {#x,(size_t)x}
#define DEF_FUNC_INFO2(x)    {"",(size_t)x}
struct func_info {
    const char * func_name;
    size_t func_ptr;
};

class CheckJumpCode {

public :
    void init();
protected:
    std::vector<struct func_info> func_infos;
};


#endif //PUREHEROTESTAPP00_CHECKJUMPCODE_H
