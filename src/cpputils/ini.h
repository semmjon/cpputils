//
// Created by sgeist on 25.02.22.
//

#ifndef CPPUTILS_INI_H
#define CPPUTILS_INI_H

#include "string_operations.h"
#include "system_operations.h"

namespace ini {
    // constants
    inline const char COMMENT_CHAR = '#';
    inline const char NEWVALUE = '=';
    inline const char NEWLINE = system_operations::LINE_SEPERATOR[std::strlen(system_operations::LINE_SEPERATOR) - 1];
    inline const char WHITESPACE = ' ';
    inline const std::string SECTION_OPEN_CHAR = "[";
    inline const std::string SECTION_CLOSE_CHAR = "]";
    inline const std::string BEGIN_PATTERN = system_operations::LINE_SEPERATOR + SECTION_OPEN_CHAR;
    py::dict ini_load(const std::map<std::string, std::vector<std::string> > &files,
                      const std::map<std::string, std::vector<std::string> > &sections,
                      const std::map<std::string, std::vector<std::string> > &keys,
                      const std::map<std::string, std::vector<py::object> > &defaults);
}

#endif //CPPUTILS_INI_H
