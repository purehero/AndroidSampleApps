//
// Created by purehero on 2024-01-09.
//

#include "MapsFile.h"
#include "jni_helper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vector>
#include <sys/mman.h>

void parse_maps( maps_data_callback func ) {
    FILE * fp = fopen( "/proc/self/maps", "r");
    if (fp != NULL) {
        MAPS_DATA maps_data;

        char line[256];
        while (fgets(line, 256, fp) != NULL) {
            memset( &maps_data, 0, sizeof(MAPS_DATA));

            int ret = sscanf(line, "%lx-%lx %c%c%c%c %llx %x:%x %lu %[^\n]\n",
                             &maps_data.start_address, &maps_data.end_address,
                             &maps_data.r, &maps_data.w, &maps_data.x, &maps_data.s,
                             &maps_data.pgoff,
                             &maps_data.major, &maps_data.minor,
                             &maps_data.inode,
                             maps_data.path);
            if (ret < 10)
            {
                fprintf(stderr, "ret = %d\n", ret);
                fprintf(stderr, "%s: sscanf failed\n", __func__);
                // check by human, if correct
                LOGE("%lx-%lx %c%c%c%c %llx %x:%x %lu %s",
                     maps_data.start_address, maps_data.end_address,
                     maps_data.r, maps_data.w, maps_data.x, maps_data.s,
                     maps_data.pgoff,
                     maps_data.major, maps_data.minor,
                     maps_data.inode,
                     maps_data.path);
                return;
            }

            if (func!=NULL) {
                (*func)( &maps_data );
            }
        }
        fclose(fp);
    }
}

#define PATH_DALVIK_CACHE       "/data/dalvik-cache/"
#define PATH_MISC               "/data/misc/"
#define PATH_SYSTEM             "/system/"
#define PATH_DEV                "/dev/"

const unsigned char dex_signature[] = { 0x64, 0x65, 0x78, 0x0a, 0x30, 0xff, 0xff, 0x00 };

void MapsDataCallbackMethod( MAPS_DATA * maps_data )
{
    static const int system_string_len = strlen(PATH_SYSTEM);
    static const int dalvik_string_len = strlen(PATH_DALVIK_CACHE);
    static const int misc_string_len = strlen(PATH_MISC);
    static const int dev_string_len = strlen(PATH_DEV);

    if ( strncmp( maps_data->path, PATH_SYSTEM, system_string_len) == 0 ) return;
    if ( strncmp( maps_data->path, PATH_DALVIK_CACHE, dalvik_string_len) == 0 ) return;
    if ( strncmp( maps_data->path, PATH_MISC, misc_string_len) == 0 ) return;
    if ( strncmp( maps_data->path, PATH_DEV, dev_string_len) == 0 ) return;
    if ( strstr( maps_data->path, ".so") != NULL ) return;

#if 1
    LOGD("%lx-%lx %c%c%c%c %llx %x:%x %lu %s",
         maps_data->start_address, maps_data->end_address,
         maps_data->r, maps_data->w, maps_data->x, maps_data->s,
         maps_data->pgoff,
         maps_data->major, maps_data->minor,
         maps_data->inode,
         maps_data->path);
#endif
    if (maps_data->r != 'r') return;


    unsigned char * base = (unsigned char *) maps_data->start_address;
    size_t addr_size = maps_data->end_address - maps_data->start_address;

    for ( size_t i = 0; i < addr_size; i++ ) {
        if ( base[i] != dex_signature[0] ) continue;
        if ( base[i+1] != dex_signature[1] ) continue;
        if ( base[i+2] != dex_signature[2] ) continue;
        if ( base[i+3] != dex_signature[3] ) continue;
        if ( base[i+4] != dex_signature[4] ) continue;
        if ( base[i+7] != dex_signature[7] ) continue;

        int dex_size = *((int *)&base[i+0x20]);
        LOGE( "found dex signature %s[%p] size:0x%x", maps_data->path, &base[i], dex_size );

        if (maps_data->w != 'w') {
            int prot = PROT_WRITE;
            prot |= maps_data->r == 'r' ? PROT_READ : 0;
            prot |= maps_data->x == 'x' ? PROT_EXEC : 0;

            mprotect((void *) maps_data->start_address, addr_size, prot );
            maps_data->w = 'w';
        }
        memset( &base[i], 0xff, 8 );        // dex signature clear
        memset( &base[i+0x20], 0x00, 4 );   // dex size clear

        i += 4;
    }
}