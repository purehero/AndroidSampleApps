//
// Created by purehero on 2024-01-12.
//

#include "WatchProcSelf.h"

#include <sys/types.h>
#include <sys/inotify.h>
#include <thread>

#define EVENT_SIZE      ( sizeof (struct inotify_event) )
#define BUF_LEN         ( 1024 * ( EVENT_SIZE + 16 ) )

bool WatchProcSelf::Init() {
    thread t(RunMethod());
}

void WatchProcSelf::RunMethod() {
    int inotify_fd = inotify_init();
    int wd = inotify_add_watch( inotify_fd, (const char*)"/proc/self", IN_ALL_EVENTS );

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
}