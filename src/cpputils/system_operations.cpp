#include "system_operations.h"

namespace system_operations{

/**
 * Checks if a file or folder or something else (mode 1: file only, 2: dir only, 3: all) exists
 * @param path path to the folder to check.
 * @return true if the folder exists, false otherwise.
 */
    bool exists(const std::string& path) {
        struct stat buffer{};
        return(stat(path.c_str(), &buffer) == 0);
    }

    bool dir_exists(const std::string& path) {
        struct stat buffer{};
        if(stat(path.c_str(), &buffer) != 0) return(false);
        return(buffer.st_mode & S_IFDIR);
    }

    bool file_exists(const std::string& path) {
        struct stat buffer{};
        if(stat(path.c_str(), &buffer) != 0) return(false);
        return(buffer.st_mode & S_IFREG);
    }

/**
 * Portable wrapper for mkdir. Internally used by mkdir()
 * @param[in] path the full path of the directory to create.
 * @return zero on success, otherwise -1.
 */
    inline int _mkdir(const char *path)
    {
#ifdef _WIN32
        return ::_mkdir(path);
#else
#if _POSIX_C_SOURCE
        return ::mkdir(path, 0755);
#else
        return ::mkdir(path, 0755); // not sure if this works on mac
#endif
#endif
    }

/**
 * Recursive, portable wrapper for mkdir.
 * @param[in] path the full path of the directory to create.
 * @return zero on success, otherwise -1.
 */
    inline int mkdir(const char *path)
    {
        std::string current_level;
        std::string level;
        std::stringstream ss(path);

        // split path using slash as a separator
        while (std::getline(ss, level, FILE_SEPERATOR))
        {
            current_level += level; // append folder to the current level

            // create current level
            if (!exists(current_level) && _mkdir(current_level.c_str()) != 0)
                return -1;

            current_level += FILE_SEPERATOR; // don't forget to append a slash
        }

        return 0;
    }

    std::string read_file(const std::string& file){
        try {
            std::ifstream in(file);
            std::string contents;
            in.seekg(0, std::ios::end);
            contents.resize(in.tellg());
            in.seekg(0, std::ios::beg);
            in.read(&contents[0], contents.size());
            in.close();
            if(&contents.back() != LINE_SEPERATOR){ // unexpected eof
                contents.append(LINE_SEPERATOR);
            }
            return(contents);
        } catch (const std::exception& e) {
            std::cout << file + " not a valid file!\n" +
                         "Check the path and permissions." << std::endl;
            exit(0);
        }
    }

}
