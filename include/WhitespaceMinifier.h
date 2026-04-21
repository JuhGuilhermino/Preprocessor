#ifndef WHITESPACE_MINIFIER_H
#define WHITESPACE_MINIFIER_H

#include <string>

class WhitespaceMinifier {
    std::string ltrim(const std::string& s);
    std::string rtrim(const std::string& s);
    std::string mtrim(const std::string& s);
public:
    std::string minify(const std::string& code);
};

#endif