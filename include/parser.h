#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include <stdexcept>
#include "lexer.h"


class Parser {
    std::vector<token> tokens;
    int current = 0;

    token peek();
    token peekSpecific(int i);
    token previous();
    token advance();
    bool finished();

    bool check(token_types_e type); // apenas olha
    bool checkNext(token_types_e type);
    bool checkValue(const std::string& lex);
    bool checkNextValue(const std::string& lex);
    bool match(token_types_e type); // olha e anda
    bool matchValue(const std::string& value);
    token consume(token_types_e type, std::string error);
    token consumeValue(const std::string& value, std::string error);

    void block();
    void methodBlock();

    void mainClassDeclaration();
    void classDeclaration();
    void multipleClassDeclarations();

    bool type();
    bool isTypeStart();

    void varDeclaration();
    void multipleVarsDeclarations();

    void methodDeclaration();
    void multipleMethodsDeclarations();
    void args();

    void commands();
    void ifCommand();
    void whileCommand();
    void printCommand();
    void assignmentCommand();

    void primaryExpression();
    void postfixExpression();
    void unaryExpression();
    void multiplicationExpression();
    void additionDifferenceExpression();
    void comparisonExpression();
    void andExpression();
    void expression();
    void listExpression();




    public:
    Parser(const std::vector<token>& tokens) : tokens(tokens) {}
    void parse();
};



#endif
