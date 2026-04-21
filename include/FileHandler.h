#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <string>
#include <vector>

class FileHandler {
public:
    static std::vector<std::string> readFile(const std::string& path);
};

#endif