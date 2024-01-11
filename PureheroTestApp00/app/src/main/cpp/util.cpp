//
// Created by purehero on 2023-12-13.
//

#include "util.h"
#include "jni_helper.h"
#include <iomanip>

int bytesToHexString( unsigned char * byteArray, int len, std::string & result ) {
    std::stringstream ss;
    ss << std::hex;

    for (int i(0) ; i < len; ++i) {
        ss << std::setw(2) << std::setfill('0') << (int) byteArray[i];
    }

    result = ss.str();
    return (int) result.size();
}