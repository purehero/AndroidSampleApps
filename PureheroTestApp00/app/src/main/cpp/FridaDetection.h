//
// Created by purehero on 2024-02-05.
//

#ifndef PUREHEROTESTAPP00_FRIDADETECTION_H
#define PUREHEROTESTAPP00_FRIDADETECTION_H

#include "utils/Inotify.h"

class FridaDetection {
public:
    FridaDetection();

    bool Init();
    bool Start();
    void Stop();
    void Release();

protected:

};

void CheckFridaInotifyEventCallback( struct inotify_event * event );

#endif //PUREHEROTESTAPP00_FRIDADETECTION_H
