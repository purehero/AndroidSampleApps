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

typedef struct func_info {
    const char * func_name;
    size_t func_ptr;
} FUNCTION_INFO;

class CheckJumpCode {
public :
    static CheckJumpCode * CreateInstanceAllMethos();
    FUNCTION_INFO * CheckAllMethod();

    CheckJumpCode(){}
    bool CheckMethod( unsigned char* ptr );
    bool CheckExitMethod() { return CheckJumpCode((unsigned char *) exit) || CheckJumpCode((unsigned char *) _exit); };

protected:
    std::vector<FUNCTION_INFO> func_infos;
    void AddAllMethod();
};


#endif //PUREHEROTESTAPP00_CHECKJUMPCODE_H
