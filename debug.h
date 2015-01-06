/*!
 * Here we define the macros that print out debug messages when
 * MAGGIE_DEBUG_LEVEL is > 0.
 *
 * These routines print fancier messages than the ones in C, but require C++ code.
 *
 * \author Arnaud Ramey
 * \file debug.h
 */

#ifndef _MAGGIE_DEBUG_H_
#define _MAGGIE_DEBUG_H_

//////////////////////////////////////////////////////////////////////////////
// Std
#include <stdio.h> // for fprintf

// Debug level
#define MAGGIE_DEBUG_LEVEL 2
// 0 : does not print any maggieDebug*()
// 1 : prints only maggieDebug1(), but not maggieDebug2() or maggieDebug3()
// 2 : prints maggieDebug1() and maggieDebug2(), but not maggieDebug3()
// 3 : prints maggieDebug1(), maggieDebug2(), maggieDebug3()

//Colors
#define PRINTF_FORMAT_NO_ATTRIB 0
#define PRINTF_FORMAT_BOLD 1
#define PRINTF_FORMAT_BLACK 30
#define PRINTF_FORMAT_RED 31
#define PRINTF_FORMAT_GREEN 32
#define PRINTF_FORMAT_YELLOW 33
#define PRINTF_FORMAT_BLUE 34
#define PRINTF_FORMAT_MAGENTA 35
#define PRINTF_FORMAT_CYAN 36
#define PRINTF_FORMAT_WHITE 37

// comment for remove colors from the maggieDebug() functions
#define USE_PRINTF_COLORS 

/*! Stringified __LINE__ */
#define MAGGIE_DEBUG_STRINGIFY(x) #x
#define MAGGIE_DEBUG_TO_STRING(x) MAGGIE_DEBUG_STRINGIFY(x)

/*!
 * an extension of printf, with the call position
 */
#define maggiePrint( ... ) MAGGIE_DEBUG(PRINTF_FORMAT_BOLD,      PRINTF_FORMAT_RED, __VA_ARGS__)

/*!
 *  Define debug() for each level of verbosity.
 *  Accepts message in the printf like fashion.
 */
#if MAGGIE_DEBUG_LEVEL == 0
#define maggieDebug1(...)  {}
#define maggieDebug2(...)  {}
#define maggieDebug3(...)  {}

#elif MAGGIE_DEBUG_LEVEL == 1
#define maggieDebug1(...)  MAGGIE_DEBUG(PRINTF_FORMAT_BOLD,      PRINTF_FORMAT_GREEN, __VA_ARGS__)
#define maggieDebug2(...)  {}
#define maggieDebug3(...)  {}

#elif MAGGIE_DEBUG_LEVEL == 2
#define maggieDebug1(...)  MAGGIE_DEBUG(PRINTF_FORMAT_BOLD,      PRINTF_FORMAT_GREEN, __VA_ARGS__)
#define maggieDebug2(...)  MAGGIE_DEBUG(PRINTF_FORMAT_BOLD,      PRINTF_FORMAT_BLUE,  __VA_ARGS__)
#define maggieDebug3(...)  {}

#elif MAGGIE_DEBUG_LEVEL == 3
#define maggieDebug1(...)  MAGGIE_DEBUG(PRINTF_FORMAT_BOLD,      PRINTF_FORMAT_GREEN, __VA_ARGS__)
#define maggieDebug2(...)  MAGGIE_DEBUG(PRINTF_FORMAT_BOLD,      PRINTF_FORMAT_BLUE,  __VA_ARGS__)
#define maggieDebug3(...)  MAGGIE_DEBUG(PRINTF_FORMAT_NO_ATTRIB, PRINTF_FORMAT_BLUE,  __VA_ARGS__)
#endif


//////////////////////////////////////////////////////////////////////////////

/*!
 * The primitive macro for debugging.
 * Accepts message in the printf like fashion.
 */
#if defined(__cplusplus) //////////////////////////////////////////////////////

#include <string>
#define MAGGIE_FILE_POSITION \
        (  ((std::string) __FILE__).substr(1 + ((std::string) __FILE__).find_last_of('/')) \
        + ":" + MAGGIE_DEBUG_TO_STRING(__LINE__) ) \
 

#ifdef __GNUC__
#define __MAGGIE_HERE__ ( std::string ("[") + MAGGIE_FILE_POSITION + std::string (" (") + std::string(__FUNCTION__) + std::string(")] \t") )
#else
#define __MAGGIE_HERE__ ( std::string ("[") + MAGGIE_FILE_POSITION + std::string ("] \t") )
#endif


#undef MAGGIE_DEBUG
#ifdef USE_PRINTF_COLORS
#define MAGGIE_DEBUG(printf_attr, printf_color, ...) { \
    printf("%c[%d;%dm%s", 0x1B, printf_attr, printf_color, __MAGGIE_HERE__.c_str()); \
	printf("%c[%dm", 0x1B, PRINTF_FORMAT_NO_ATTRIB); \
    printf(__VA_ARGS__); \
    printf("\n"); \
}
#else  // USE_PRINTF_COLORS == false
#define MAGGIE_DEBUG(printf_attr, printf_color, ...) { \
    printf("%s", __MAGGIE_HERE__.c_str()); \
    printf(__VA_ARGS__); \
    printf("\n"); \
}
#endif // USE_PRINTF_COLORS


#else // we compile C code ////////////////////////////////////////////////////

#ifdef USE_PRINTF_COLORS
#define MAGGIE_DEBUG(printf_attr, printf_color, ...) { \
    printf("%c[%d;%dm[" __FILE__ ":" MAGGIE_DEBUG_TO_STRING(__LINE__) "] ", 0x1B, printf_attr, printf_color); \
	printf("%c[%dm", 0x1B, PRINTF_FORMAT_NO_ATTRIB); \
    printf(__VA_ARGS__); \
    printf("\n"); \
}
#else  // USE_PRINTF_COLORS == false
#define MAGGIE_DEBUG(printf_attr, printf_color, ...) { \
    printf("[" __FILE__ ":" MAGGIE_DEBUG_TO_STRING(__LINE__) "] " __VA_ARGS__); \
    printf("\n"); \
}
#endif // USE_PRINTF_COLORS

#endif // defined(__cplusplus) ////////////////////////////////////////////////

#endif /* _MAGGIE_DEBUG_H_ */

