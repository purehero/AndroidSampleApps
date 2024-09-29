//
// Created by purehero on 2024-02-05.
//

#include "FridaDetection.h"
#include "jni_helper.h"

FridaDetection::FridaDetection()
{

}

bool FridaDetection::Init()
{

}

bool FridaDetection::Start()
{

}

void FridaDetection::Stop()
{

}

void FridaDetection::Release()
{

}

void CheckFridaInotifyEventCallback( struct inotify_event * event )
{
    if (event->len) {
        bool need_check = event->name[0] == 'a' && event->name[1] == 'u' && event->name[2] == 'x' && event->name[3] == 'v';
        if (!need_check) {
            need_check = event->name[0] == 'm' && event->name[1] == 'a' && event->name[2] == 'p' && event->name[3] == 's';

            if (!need_check) {
                need_check = event->name[0] == 's' && event->name[1] == 'm' && event->name[2] == 'a' && event->name[3] == 'p';
            }
        }

        if (need_check/* && CheckExitJumpCode()*/) {
            LOGE("DETECTED Frida!!!!");
#if 0   // 잠시 종료 시키지 않는다.
            exit(0);
            _exit(-1);
#endif
        }

        if (event->mask & IN_CREATE) {
            LOGD(" ==> %s [IN_CREATE]", event->name);
        } else if (event->mask & IN_DELETE) {
            LOGD(" ==> %s [IN_DELETE]", event->name);
        } else if (event->mask & IN_MODIFY) {
            LOGD(" ==> %s [IN_MODIFY]", event->name);
        } else if (event->mask & IN_OPEN) {
            LOGD(" ==> %s [IN_OPEN]", event->name);
        } else if (event->mask & IN_CLOSE_WRITE) {
            LOGD(" ==> %s [IN_CLOSE_WRITE]", event->name);
        } else if (event->mask & IN_CLOSE_NOWRITE) {
            LOGD(" ==> %s [IN_CLOSE_NOWRITE]", event->name);
        } else if (event->mask & IN_CLOSE) {
            LOGD(" ==> %s [IN_CLOSE]", event->name);
        }
    }
