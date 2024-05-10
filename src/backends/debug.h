#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include <sstream>

// Uncomment the line below in any file or define this in your build system to enable debug output
#define DEBUG

#ifdef DEBUG
    #define DEBUG_ENABLED 1
#else
    #define DEBUG_ENABLED 0
#endif

// Basic debug message logging
#define DEBUG_LOG(msg) \
    do { \
        if (DEBUG_ENABLED) { \
            std::ostringstream os_; \
            os_ << "DEBUG: " << __FILE__ << "(" << __LINE__ << ") " << msg << std::endl; \
            std::cerr << os_.str(); \
        } \
    } while (0)

// Debug logging with variable information
#define DEBUG_VAR(var) \
    do { \
        if (DEBUG_ENABLED) { \
            std::ostringstream os_; \
            os_ << "DEBUG: " << __FILE__ << "(" << __LINE__ << ") " #var " = " << (var) << std::endl; \
            std::cerr << os_.str(); \
        } \
    } while (0)

// Assertion with debug
#define DEBUG_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            std::ostringstream os_; \
            os_ << "ASSERTION FAILED: " << __FILE__ << "(" << __LINE__ << ") " \
                << msg << " [" #cond "]" << std::endl; \
            std::cerr << os_.str(); \
            std::abort(); \
        } \
    } while (0)

#endif // DEBUG_H

