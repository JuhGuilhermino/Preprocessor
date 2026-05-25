#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <string>
#include "WhitespaceMinifier.h"
#include "CommentRemover.h"
#include "FileHandler.h"

class Preprocessor {
    WhitespaceMinifier wsm;
    FileHandler fh;
    CommentRemover cm;
    //static std::string processed_code;

public:
    std::string process(std::string& filePath);

};

#endif