//
// Created by purehero on 2024-01-09.
//

#ifndef PUREHEROTESTAPP00_MAPSFILE_H
#define PUREHEROTESTAPP00_MAPSFILE_H

typedef struct __maps_file_data__ {
    unsigned long start_address;
    unsigned long end_address;
    char r, w, x, s;
    unsigned long long pgoff;
    int major, minor; // device
    unsigned long inode;
    char path[256];
} MAPS_DATA;

typedef void (*maps_data_callback)( MAPS_DATA * maps_data );
void parse_maps( maps_data_callback func );

void MapsDataCallbackMethod( MAPS_DATA * maps_data );

#endif //PUREHEROTESTAPP00_MAPSFILE_H
