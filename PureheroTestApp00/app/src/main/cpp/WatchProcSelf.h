//
// Created by purehero on 2024-01-12.
//

#ifndef PUREHEROTESTAPP00_WATCHPROCSELF_H
#define PUREHEROTESTAPP00_WATCHPROCSELF_H

#include <string>
#include <functional>

class WatchProcSelf {
protected :
    bool Init();

private :
    void RunMethod();

protected :
    typedef std::function<void()> Callback;
    Callback handler;
public :
    void SetVoidHandler(Callback func) {
        handler = func;
    }
};


#endif //PUREHEROTESTAPP00_WATCHPROCSELF_H
