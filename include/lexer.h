#ifndef LEXER_H
#define LEXER_H

#include "TokenTypes.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>


struct token {
    token_types_e type;
    std::string value;
    int line;
    int column;
};

class Lexer {
    int current = 0;
    int line = 1;
    int column = 1;
    static std::unordered_map<token_types_e, std::vector<std::string>> tokens;
    std::vector<token> tokenList;
    
    char peek();
    char peekNext();
    void advance();
    bool finished();
    
    std::pair<std::string, token_types_e> readNumber();
    std::string readWord();
    bool verifyInMap(std::string value, token_types_e type);
    token_types_e verifyWord(std::string word);
    
    public:
    void setSource(std::string source) { this->source = source;};
    void reset() { current = 0; line = 1; column = 1; tokenList.clear();};
    std::string source;
    void mainLoop(bool stop_error = false);
    std::vector<token> getTokenList() { return tokenList;};
};

#endif
