#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <string>

class FileHandler {
public:
    static std::string readFile(const std::string& path);
    static void writeFile(const std::string& path, const std::string& content);
};

#endif