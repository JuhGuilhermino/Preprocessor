#include "Preprocessor.h"
#include <iostream>


std::string Preprocessor::process(std::string& filePath){
    auto lines = fh.readFile(filePath);
    
    std::string code;
    for (auto& line : lines) {
        code += line + "\n";
    }

    std::string nocomments = cm.remove(code);

    std::string result;
    std::string temp;

    for (char c : noComments) {
        if (c == '\n') {
            result += wsm.minify(temp) + "\n";
            temp.clear();
        } else {
            temp += c;
        }
    }

    return result;

}
