#ifndef WHITESPACE_MINIFIER_H
#define WHITESPACE_MINIFIER_H

#include <string>

class WhitespaceMinifier {
public:
    std::string minify(const std::string& code);
};

#endif