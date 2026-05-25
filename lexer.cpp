#include "lexer.h"

#include <cctype>
#include <algorithm>
#include <iostream>


std::unordered_map<token_types_e, std::vector<std::string>> Lexer::tokens = {
    {token_types_e::RESERVED_WORD, {"class", "if", "else", "return", "void", "static", "public", "while", "new", "this", "boolean", "int"}},
    {token_types_e::IDENTIFIER, {}},
    {token_types_e::OPERATORS, {"=", "==", "+", "*", "<", ">", "-", "!=", "<=", ">=", "&&", "||", "++", "--"}},
    {token_types_e::INTEGER, {}},
    {token_types_e::DELIMITERS, {";", ",", ".", "(", ")", "{", "}", "[", "]"}},
    {token_types_e::OTHERS, {}},
    {token_types_e::FLOAT, {}}
};


char Lexer::peek(){
    if(finished()){
        return '\0';
    }
    return source[current];
}

char Lexer::peekNext(){
    if(current + 1 >= source.size()){
        return '\0';
    }
    return source[current+1];
}

void Lexer::advance(){
    current++;
}

bool Lexer::finished(){
    return current >= source.size();
}

std::pair<std::string, token_types_e> Lexer::readNumber(){
    std::string number;
    bool isFloat = false;

    while(isdigit(peek())){
        number += peek();
        advance();
    }

    if(peek() == '.' && isdigit(peekNext())){
        isFloat = true;
        number += peek();
        advance();

        while(isdigit(peek())){
            number += peek();
            advance();
        }
    }
    
    if(isFloat){
        return std::make_pair(number, token_types_e::FLOAT);
    }

    return std::make_pair(number, token_types_e::INTEGER);


}


token_types_e Lexer::verifyWord(std::string word){
    auto it = std::find(tokens[token_types_e::RESERVED_WORD].begin(), tokens[token_types_e::RESERVED_WORD].end(), word);

    if (it != tokens[token_types_e::RESERVED_WORD].end()) {
        return token_types_e::RESERVED_WORD;
    }

    return token_types_e::IDENTIFIER;
}

std::string Lexer::readWord(){
    std::string word;
    while(isalnum(peek()) || peek() == '_'){
        word += peek();
        advance();
    }  

    return word;

}

bool Lexer::verifyInMap(std::string value, token_types_e type){
    auto it = std::find(tokens[type].begin(), tokens[type].end(), value);

    if (it != tokens[type].end()) {
        return true;
    }

    return false;

}

void Lexer::mainLoop(){
    reset();
    while(!finished()){
        std::cout << "char: '" << peek() << "' idx: " << current << std::endl;

        char currentChar = peek();

        if(isspace(currentChar)){
            if(currentChar == '\n'){
                ++line;
            }
            advance();
        }
        else if (isdigit(currentChar)){
            auto number = readNumber();
            tokenList.push_back({number.second, number.first, line});
        }
        else if (isalpha(currentChar) || currentChar == '_'){
            auto word = readWord();
            auto type = verifyWord(word);

            if(type == token_types_e::RESERVED_WORD){
                tokenList.push_back({token_types_e::RESERVED_WORD, word, line });
            } 
            else if (type == token_types_e::IDENTIFIER){
                tokenList.push_back({token_types_e::IDENTIFIER, word, line });
            }
            
        } else {
            std::string c(1, currentChar);

            if(verifyInMap(c, token_types_e::DELIMITERS)){
                tokenList.push_back({token_types_e::DELIMITERS, c, line });
                advance();
            } else if (verifyInMap(c, token_types_e::OPERATORS)){
                std::string twoChar = c + peekNext();

                if(verifyInMap(twoChar, token_types_e::OPERATORS)){
                    tokenList.push_back({token_types_e::OPERATORS, twoChar, line});
                    advance();
                    advance();
                } else {
                    tokenList.push_back({token_types_e::OPERATORS, c, line});
                    advance();
                }
            } else {
                tokenList.push_back({token_types_e::OTHERS, c, line});
                advance();
            }

        }

    }
    tokenList.push_back({token_types_e::EOF_TOKEN, "", line});

}
