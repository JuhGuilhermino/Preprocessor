#include "lexer.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <stdexcept>


// Adicionei 'extends' e 'lenght' as palavras reservadas para admitir a herança
std::unordered_map<token_types_e, std::vector<std::string>> Lexer::tokens = {
    {token_types_e::RESERVED_WORD, {"class", "if", "else", "return", "void", "static", "public", "while", "new", "this", "boolean", "int", "extends", "length"}},
    {token_types_e::IDENTIFIER, {}},
    {token_types_e::OPERATORS, {"=", "+", "*", "<", ">", "!", "-", "&&"}},
    {token_types_e::INTEGER, {}},
    {token_types_e::DELIMITERS, {";", ",", ".", "(", ")", "{", "}", "[", "]"}}};

char Lexer::peek()
{
    if (finished())
    {
        return '\0';
    }
    return source[current];
}

char Lexer::peekNext()
{
    if (current + 1 >= source.size())
    {
        return '\0';
    }
    return source[current + 1];
}

void Lexer::advance()
{
    if (finished())
    {
        return;
    }

    if (source[current] == '\n')
    {
        ++line;
        column = 1;
    }
    else
    {
        ++column;
    }

    current++;
}

bool Lexer::finished()
{
    return current >= source.size();
}

std::pair<std::string, token_types_e> Lexer::readNumber()
{
    std::string number;

    while (isdigit(peek()))
    {
        number += peek();
        advance();
    }
    return std::make_pair(number, token_types_e::INTEGER);
}

token_types_e Lexer::verifyWord(std::string word)
{
    auto it = std::find(tokens[token_types_e::RESERVED_WORD].begin(), tokens[token_types_e::RESERVED_WORD].end(), word);

    if (it != tokens[token_types_e::RESERVED_WORD].end())
    {
        return token_types_e::RESERVED_WORD;
    }

    return token_types_e::IDENTIFIER;
}

std::string Lexer::readWord()
{
    std::string word;
    while (isalnum(peek()) || peek() == '_')
    {
        word += peek();
        advance();
    }

    return word;
}

bool Lexer::verifyInMap(std::string value, token_types_e type)
{
    auto it = std::find(tokens[type].begin(), tokens[type].end(), value);

    if (it != tokens[type].end())
    {
        return true;
    }

    return false;
}

void Lexer::mainLoop(bool stop_error)
{
    reset();
    std::vector<std::string> lexicalErrors;

    while (!finished())
    {
        char currentChar = peek();
        int tokenLine = line;
        int tokenColumn = column;

        if (isspace(currentChar))
        {
            advance();
        }
        else if (currentChar == '/' && peekNext() == '/')
        {
            while (!finished() && peek() != '\n')
            {
                advance();
            }
        }
        else if (currentChar == '/' && peekNext() == '*')
        {
            advance();
            advance();

            while (!finished() && !(peek() == '*' && peekNext() == '/'))
            {
                advance();
            }

            if (finished())
            {
                std::string error_message = "Comentario de bloco nao foi devidamente fechado na linha "
                    + std::to_string(tokenLine) + ", coluna " + std::to_string(tokenColumn);
                if (stop_error)
                {
                    throw std::runtime_error(error_message);
                }
                lexicalErrors.push_back(error_message);
                //break;   // Para permitir que sejam retornados multiplos erros certo
            }

            advance();
            advance();
        }
        else if (isdigit(currentChar))
        {
            auto number = readNumber();
            tokenList.push_back({number.second, number.first, tokenLine, tokenColumn});
        }
        else if (isalpha(currentChar) || currentChar == '_')
        {
            auto word = readWord();
            auto type = verifyWord(word);

            if (type == token_types_e::RESERVED_WORD)
            {
                tokenList.push_back({token_types_e::RESERVED_WORD, word, tokenLine, tokenColumn});
            }
            else if (type == token_types_e::IDENTIFIER)
            {
                tokenList.push_back({token_types_e::IDENTIFIER, word, tokenLine, tokenColumn});
            }
        }
        else
        {
            std::string c(1, currentChar);
            std::string twoChar = c + peekNext();

            
            if (verifyInMap(twoChar, token_types_e::OPERATORS)) // Troquei a ordem para verificar o && primeiro
            {
                tokenList.push_back({token_types_e::OPERATORS, twoChar, tokenLine, tokenColumn});
                advance();
                advance();
            }
            else if (verifyInMap(c, token_types_e::DELIMITERS)) 
            {
                tokenList.push_back({token_types_e::DELIMITERS, c, tokenLine, tokenColumn});
                advance();
            }
            else if (verifyInMap(c, token_types_e::OPERATORS)) 
            {
                tokenList.push_back({token_types_e::OPERATORS, c, tokenLine, tokenColumn});
                advance();
            }
            else
            {
                std::string error_message = std::string("token '") + currentChar + "' invalido na linha "
                    + std::to_string(tokenLine) + ", coluna " + std::to_string(tokenColumn);
                if (stop_error)
                {
                    throw std::runtime_error(error_message);
                }
                lexicalErrors.push_back(error_message);
                advance();
            }
        }
    }

    if (!lexicalErrors.empty())
    {
        std::string error_message = lexicalErrors.front();
        for (size_t i = 1; i < lexicalErrors.size(); ++i)
        {
            error_message += "\n";
            error_message += lexicalErrors[i];
        }
        throw std::runtime_error(error_message);
    }

    tokenList.push_back({token_types_e::EOF_TOKEN, "", line, column});
}
