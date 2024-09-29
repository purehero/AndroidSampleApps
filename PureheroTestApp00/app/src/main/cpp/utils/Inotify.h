//
// Created by purehero on 2024-02-05.
//

#ifndef PUREHEROTESTAPP00_INOTIFY_H
#define PUREHEROTESTAPP00_INOTIFY_H

#include <sys/inotify.h>
#include <vector>
#include <thread>

using InotifyCallback = void(*)( struct inotify_event * event );

class Inotify {
public :
    Inotify();

    bool Init( const char * path );
    bool Start();
    void Stop();
    void Release();

    void RegisterCallback( InotifyCallback cb );
protected:
    int fd, wd;
    bool run_flag;
    std::thread thread_id;
    std::vector<InotifyCallback> callbacks;

    void Run();
};


#endif //PUREHEROTESTAPP00_INOTIFY_H
