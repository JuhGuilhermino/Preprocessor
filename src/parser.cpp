#include "parser.h"

token Parser::peek()
{
    if (tokens.empty())
    {
        return {token_types_e::EOF_TOKEN, "", 0};
    }

    if (current >= tokens.size())
    {
        return tokens.back();
    }

    return tokens[current];
}

token Parser::peekSpecific(int i)
{
    if (tokens.empty())
    {
        return {token_types_e::EOF_TOKEN, "", 0};
    }

    if (i >= tokens.size())
    {
        return tokens.back();
    }

    return tokens[i];
}

token Parser::previous()
{
    if (tokens.empty())
    {
        return {token_types_e::EOF_TOKEN, "", 0};
    }

    if (current == 0)
    {
        return tokens.front();
    }

    return tokens[current - 1];
}

token Parser::advance()
{
    if (!finished())
    {
        current++;
    }

    return previous();
}

bool Parser::finished()
{
    return peek().type == token_types_e::EOF_TOKEN;
}

bool Parser::check(token_types_e type)
{
    return peek().type == type;
}

bool Parser::checkValue(const std::string &lex)
{
    return peek().value == lex;
}

bool Parser::checkNext(token_types_e type)
{
    auto next = current + 1;
    return peekSpecific(next).type == type;
}

bool Parser::checkNextValue(const std::string &lex)
{
    auto next = current + 1;
    return peekSpecific(next).value == lex;
}

bool Parser::match(token_types_e type)
{
    if (check(type))
    {
        advance();
        return true;
    }

    return false;
}

bool Parser::matchValue(const std::string &value)
{
    if (checkValue(value))
    {
        advance();
        return true;
    }

    return false;
}

token Parser::consume(token_types_e type, std::string error)
{
    if (check(type))
    {
        return advance();
    }

    throw std::runtime_error(error);
}

token Parser::consumeValue(const std::string &value, std::string error)
{
    if (checkValue(value))
    {
        return advance();
    }
    throw std::runtime_error(error);
}
void Parser::block()
{
    consumeValue("{", "Expected {");

    while (!checkValue("}") && !finished())
    {
        commands();
    }

    consumeValue("}", "Expected }");
}

void Parser::ifCommand()
{
    consumeValue("if", "Expected if");
    consumeValue("(", "Expected (");
    expression();
    consumeValue(")", "Expected )");
    commands();
    consumeValue("else", "Expected else");

    if (checkValue("if"))
    {
        ifCommand();
    }
    else
    {
        commands();
    }
}

void Parser::whileCommand()
{
    consumeValue("while", "Expected while");
    consumeValue("(", "Expected (");
    expression();
    consumeValue(")", "Expected )");
    commands();
}

void Parser::printCommand()
{
    consumeValue("System", "Expected System");
    consumeValue(".", "Expected .");
    consumeValue("out", "Expected out");
    consumeValue(".", "Expected .");
    consumeValue("println", "Expected println");
    consumeValue("(", "Expected (");
    expression();
    consumeValue(")", "Expected )");
    consumeValue(";", "Expected ;");
}

void Parser::assignmentCommand()
{
    consume(token_types_e::IDENTIFIER, "Expected identifier");

    if (checkValue("["))
    {
        consumeValue("[", "Expected [");
        expression();
        consumeValue("]", "Expected ]");
        consumeValue("=", "Expected =");
        expression();
        consumeValue(";", "Expected ;");
    }
    else
    {
        consumeValue("=", "Expected =");
        expression();
        consumeValue(";", "Expected ;");
    }
}

void Parser::commands()
{
    if (checkValue("{"))
    {
        block();
    }
    else if (checkValue("if"))
    {
        ifCommand();
    }
    else if (checkValue("while"))
    {
        whileCommand();
    }
    else if (checkValue("System"))
    {
        printCommand();
    }
    else if (check(token_types_e::IDENTIFIER))
    {
        assignmentCommand();
    }
    else
    {
        throw std::runtime_error("Expected command");
    }
}

void Parser::commandList()
{
    while (checkValue("{") || checkValue("if") || checkValue("while") ||
           checkValue("System") || check(token_types_e::IDENTIFIER))
    {
        commands();
    }
}

void Parser::methodBlock()
{
    consumeValue("{", "Expected '{'");
    multipleVarsDeclarations();
    commandList();
    consumeValue("return", "Expected return");
    expression();
    consumeValue(";", "Expected ;");
    consumeValue("}", "Expected }");
}

void Parser::args()
{
    type();
    consume(token_types_e::IDENTIFIER, "Expected arg name");

    while (checkValue(","))
    {
        consumeValue(",", "Expected ,");
        type();
        consume(token_types_e::IDENTIFIER, "Expected arg name");
    }
}

void Parser::methodDeclaration()
{
    consumeValue("public", "Expected public");
    type();
    consume(token_types_e::IDENTIFIER, "Exepected method identifier");
    consumeValue("(", "Expected (");

    if (!checkValue(")"))
    {
        args();
    }

    consumeValue(")", "Expected )");
    methodBlock();
}

void Parser::multipleMethodsDeclarations()
{
    while (checkValue("public"))
    {
        methodDeclaration();
    }
}

void Parser::mainClassDeclaration()
{
    consumeValue("class", "Expected 'class'");

    consume(token_types_e::IDENTIFIER, "Expected main class name");

    consumeValue("{", "Expected '{' after class name");

    consumeValue("public", "Expected 'public'");
    consumeValue("static", "Expected 'static'");
    consumeValue("void", "Expected 'void'");
    consumeValue("main", "Expected 'main'");

    consumeValue("(", "Expected '(' after main");
    consumeValue("String", "Expected 'String'");
    consumeValue("[", "Expected '[' after String");
    consumeValue("]", "Expected ']' after '['");
    consume(token_types_e::IDENTIFIER, "Expected args name");
    consumeValue(")", "Expected ')' after main parameter");

    consumeValue("{", "Expected '{' before main body");

    commandList();

    consumeValue("}", "Expected '}' after main body");

    consumeValue("}", "Expected '}' after main class");
}

void Parser::classDeclaration()
{
    consumeValue("class", "Expected 'class'");
    consume(token_types_e::IDENTIFIER, "Expected class name");

    if (matchValue("extends"))
    {
        consume(token_types_e::IDENTIFIER, "Expected class name");
    }

    consumeValue("{", "Expected '{' before main body");

    multipleVarsDeclarations();
    multipleMethodsDeclarations();

    consumeValue("}", "Expected '}' after main body");
}

void Parser::multipleClassDeclarations()
{
    while (checkValue("class"))
    {
        classDeclaration();
    }
}

bool Parser::isTypeStart()
{
    return (checkValue("int") || checkValue("boolean") || (check(token_types_e::IDENTIFIER) && checkNext(token_types_e::IDENTIFIER)));
}

bool Parser::type()
{
    if (matchValue("int"))
    {
        if (matchValue("["))
        {
            consumeValue("]", "Expected ]");
        }
        return true;
    }

    if (matchValue("boolean"))
    {
        return true;
    }

    if (match(token_types_e::IDENTIFIER))
    {
        return true;
    }

    return false;
}

void Parser::varDeclaration()
{
    if (!type())
    {
        throw std::runtime_error("Expected type");
    }

    consume(token_types_e::IDENTIFIER, "Expected var name");
    consumeValue(";", "Expected ';' after variable declaration");
}

void Parser::multipleVarsDeclarations()
{
    while (isTypeStart())
    {
        varDeclaration();
    }
}

void Parser::parse()
{
    mainClassDeclaration();
    multipleClassDeclarations();
    consume(token_types_e::EOF_TOKEN, "Expected end of file");
}

void Parser::primaryExpression()
{
    if (match(token_types_e::INTEGER))
    {
        return;
    }
    else if (matchValue("true"))
    {
        return;
    } else if (matchValue("false")){
        return;
    } else if (matchValue("this")){
        return;
    } else if (match(token_types_e::IDENTIFIER)){
        return;
    } else if (matchValue("new")){
        if(matchValue("int")){
            consumeValue("[", "Expected [");
            expression();
            consumeValue("]", "Expected ]");
            return;
        } else if (match(token_types_e::IDENTIFIER)){
            consumeValue("(", "Expected (");
            consumeValue(")", "Expected )");
            return;
        } 
    } else if (matchValue("(")){
        expression();
        consumeValue(")", "Expected )");
        return;
    }

    throw std::runtime_error("Expected expression");
}

void Parser::postfixExpression()
{
    primaryExpression();

    while (true)
    {
        if (matchValue("["))
        {
            expression();
            consumeValue("]", "Expected ]");
        }
        else if (matchValue("."))
        {
            if (matchValue("length"))
            {
                // Reconheceu .length
            }
            else
            {
                consume(token_types_e::IDENTIFIER, "Expected method name after '.'");
                consumeValue("(", "Expected ( after method name");

                listExpression();

                consumeValue(")", "Expected ) after method call");
            }
        }
        else
        {
            break;
        }
    }
}


void Parser::unaryExpression(){
    matchValue("!");
    postfixExpression();
}

void Parser::multiplicationExpression(){
    unaryExpression();

    while(matchValue("*")){
        unaryExpression();
    }
}

void Parser::additionDifferenceExpression(){
    multiplicationExpression();

    while(matchValue("+") || matchValue("-")){
        multiplicationExpression();
    }
}

void Parser::comparisonExpression(){
    additionDifferenceExpression();
    if(matchValue(">") || matchValue("<")){
        additionDifferenceExpression();
    }
}

void Parser::andExpression(){
    comparisonExpression();
    while(matchValue("&&")){
        comparisonExpression();
    }
}

void Parser::expression(){
    andExpression();
}

void Parser::listExpression(){
    if(checkValue(")")){
        return;
    }
    expression();

    while(matchValue(",")){
        expression();
    }

}
