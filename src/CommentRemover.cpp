#include "CommentRemover.h"
#include <vector>
#include <fstream>

std::string CommentRemover::remove(const std::string& code){
    std::string result;
    bool inSingleLineComment = false;
    bool inMultiLineComment = false;

    for (size_t i = 0; i < code.size(); i++) {
        if (!inSingleLineComment && !inMultiLineComment &&
            code[i] == '/' && code[i+1] == '/') {
            inSingleLineComment = true;
            i++;
        }
        else if (!inSingleLineComment && !inMultiLineComment &&
                 code[i] == '/' && code[i+1] == '*') {
            inMultiLineComment = true;
            i++;
        }
        else if (inSingleLineComment && code[i] == '\n') {
            inSingleLineComment = false;
        }
        else if (inMultiLineComment &&
                 code[i] == '*' && code[i+1] == '/') {
            inMultiLineComment = false;
            i++;
        }
        else if (!inSingleLineComment && !inMultiLineComment) {
            result += code[i];
        }
    }

    return result;
  }