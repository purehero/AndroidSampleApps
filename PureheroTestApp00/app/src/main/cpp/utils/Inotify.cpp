//
// Created by purehero on 2024-02-05.
//

#include "Inotify.h"
#include "../jni_helper.h"

#define EVENT_SIZE      (sizeof(struct inotify_event))
#define EVENT_BUF_LEN   (1024*(EVENT_SIZE+16))

Inotify::Inotify()
{
    fd = -1;
    wd = -1;
    run_flag = false;
}

bool Inotify::Init( const char * path )
{
    Release();

    fd = inotify_init();
    if ( fd < 0 ) {
        LOGE("inotify_init fail.");
        return false;
    }

    wd = inotify_add_watch(fd, path, IN_ALL_EVENTS);
    return wd != -1;
}

bool Inotify::Start()
{
    if (fd != -1 && wd != -1) {
        run_flag = true;
        thread_id = std::thread(&Inotify::Run, this);
    }
    return run_flag;
}

void Inotify::Stop()
{
    if (run_flag) {
        run_flag = false;
        thread_id.join();
    }
}

void Inotify::Release()
{
    if (fd != -1) {
        if (wd != -1) {
            inotify_rm_watch(fd, wd);
        }
        close( fd );
    }
    fd = -1;
    wd = -1;
}

void Inotify::Run()
{
    char * buffer = new char[EVENT_BUF_LEN];
    while (run_flag) {
        int i = 0;
        int length = read(fd, buffer, EVENT_BUF_LEN);

        while (i < length) {
            struct inotify_event *event = (struct inotify_event *) &buffer[i];
            for (InotifyCallback cb : callbacks) {
                (*cb)( event );
            }

            i += EVENT_SIZE + event->len;
        }
    }

    delete [] buffer;
}

void Inotify::RegisterCallback( InotifyCallback cb ) {
    callbacks.push_back(cb);
}