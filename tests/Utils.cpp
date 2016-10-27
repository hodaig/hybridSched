/*
 * Utils.cpp
 *
 *  Created on: Sep 19, 2016
 *      Author: hodai
 */

#include "Utils.h"

using namespace std;

int Utils::_identLevel = 0;

// from APM
uint64_t Utils::micros64()
{

    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return 1.0e6*(ts.tv_sec + (ts.tv_nsec*1.0e-9));
}

unsigned int Utils::getNullTerminatedArraySize(void** arr){
    unsigned int count = 0;
    while (arr[0]){
        count++;
        arr++;
    }
    return count;
}

void Utils::printTimes(const char* str, unsigned int count){
    unsigned int i;
    for (i=0; i<count ; i++){
        DEBBUG_PRINTF(str);
    }
}


vector<string> Utils::split(const char *str, char c = ' ')
{
    vector<string> result;

    do
    {
        const char *begin = str;

        while(*str != c && *str)
            str++;

        result.push_back(string(begin, str));
    } while (0 != *str++);

    return result;
}
