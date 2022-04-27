#ifndef CPPUTILS_SYSTEM_OPERATIONS_H
#define CPPUTILS_SYSTEM_OPERATIONS_H

#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

namespace system_operations {

    // Constants
    // System-Line-Seperator
    #ifdef _WIN32
        inline const char* LINE_SEPERATOR = "\r\n"
    #elif defined macintosh // OS 9
        inline const char* LINE_SEPERATOR = "\r"
    #else
        inline const char* LINE_SEPERATOR = "\n"; // Mac OS X uses \n
    #endif

    // System-File-Seperator
    #ifdef _WIN32
        inline const char FILE_SEPERATOR = '\\'
    #else
        inline const char FILE_SEPERATOR = '/'; // Mac OS X uses \n
    #endif

    bool exists(const std::string& path);
    bool dir_exists(const std::string& path);
    bool file_exists(const std::string& path);
    int mkdir(const char *path);
    std::string read_file(const std::string& file, bool erase_space);

}

#endif //CPPUTILS_SYSTEM_OPERATIONS_H
