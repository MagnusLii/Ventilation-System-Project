#ifndef DEBUGPRINT_H
#define DEBUGPRINT_H


#ifndef DEBUG_ENABLE
#define DPRINT(...)  ((void)0)
#else
#include <iostream>

namespace debugging
{
    template<typename... Args>
    void print(const char* file, int line, Args... args) {
        (std::clog << "[" << file << ":" << line << "] "
                  << ... << args) << std::endl;
    }
}

#define DPRINT(...)  debugging::print(__FILE__, __LINE__, __VA_ARGS__)
#endif

#endif
