/*
 * Utils.h
 *
 *  Created on: Sep 15, 2016
 *      Author: hodai
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <stdlib.h>             // exit()
#include <stdio.h>				// for printf();
#include <stdint.h> 		// types
#include <time.h>
#include <vector>
#include <string>

#ifdef HS_ALONE
#define MICROS() (Utils::micros64() & 0xFFFFFFFF)
#else
#define MICROS() AP_HAL::micros()
#define MICROS64() AP_HAL::micros64()

#endif

class Utils
{
public:
    static int _identLevel;
	// from APM
	static uint64_t micros64();

	//static unsigned int getNullTerminatedArraySize(void** arr);

	static void printTimes(const char* str, unsigned int count);

	static std::vector<std::string> split(const char *str, char c);

};




#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))
#endif

#define HS_UTILS_FOREACH(type, name, collection) \
            for (type##::iterator name = collection.begin(); name!=collection.end(); ++name)

/**
 * use this macro as printf() function to printing in debbug mode only
 */
#define DEBUG_VERB_FATAL      1
#define DEBUG_VERB_ERROR      2
#define DEBUG_VERB_WARNING    3
#define DEBUG_VERB_TRACE      4
#define DEBUG_VERB_LOG        5
#define DEBUG_VERB_BEDUG      6

#define DEBBUGING_MODE 3


#ifdef DEBBUGING_MODE
#   define DEBBUG_PRINTF(...) printf(__VA_ARGS__)
#   define DEBBUG_PRINTF_info(...) \
        printf("%s:%d:%s: - ", __FILE__, __LINE__, __FUNCTION__);\
        printf(" " __VA_ARGS__);\
        printf("\n");
#   define DEBBUG_PRINTF_INFO_LEVEL(level, ...) \
        if (level <= DEBBUGING_MODE) {\
            DEBBUG_PRINTF_info(__VA_ARGS__); \
        }

#   if DEBBUGING_MODE >= DEBUG_VERB_TRACE
#       define DEBUG_TRACE_START()\
        Utils::_identLevel++;\
        Utils::printTimes(">> ", Utils::_identLevel);\
        DEBBUG_PRINTF_info("start");
#       define DEBUG_TRACE_END()\
        Utils::printTimes(">> ", Utils::_identLevel);\
        DEBBUG_PRINTF_info("end");\
        Utils::_identLevel--;
#       define DEBUG_TRACE_RETURN(ret)\
        DEBUG_TRACE_END();\
        return ret;

#   else
#       define DEBUG_TRACE_START()
#       define DEBUG_TRACE_END()
#       define DEBUG_TRACE_RETURN(ret)\
            return ret;
#   endif

#else
#   define DEBBUG_PRINTF(...) /* do noting */
#   define DEBBUG_PRINTF_info(...) /* do noting */
#endif

#define ASSERT(...)\
            printf("ASSERT in: %s:%d:%s - ", __FILE__, __LINE__, __FUNCTION__);\
            printf(__VA_ARGS__);\
            printf("\n"); \
            exit(-1);




#endif /* UTILS_H_ */
