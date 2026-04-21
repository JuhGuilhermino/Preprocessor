#include "Preprocessor.h"

std::string Preprocessor::process(std::string& filePath){
    auto lines = fh.readFile(filePath);

    for(auto& line: lines){
        line = wsm.minify(line);
        processed_code += line;
    }

    return processed_code;

}
