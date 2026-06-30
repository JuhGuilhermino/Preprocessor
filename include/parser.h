#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include <stdexcept>
#include "lexer.h"
#include "ast.h"
#include "SymbolTable.h"


class Parser {
    std::vector<token> tokens;
    int current = 0;

    SymbolTable symbolTable;
    std::string currentClassName = "";
    std::string currentMethodName = "";

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

    std::unique_ptr<BlockCommandNode> block();
    void methodBlock(MethodNode& node);

    MainClassNode mainClassDeclaration();
    ClassNode classDeclaration();
    std::vector<ClassNode> multipleClassDeclarations();

    TypeNode type();
    bool isTypeStart();

    VarDeclNode varDeclaration();
    std::vector<VarDeclNode> multipleVarsDeclarations();

    MethodNode methodDeclaration();
    std::vector<MethodNode> multipleMethodsDeclarations();
    std::vector<VarDeclNode> args();

    std::unique_ptr<CommandNode> commands();
    std::vector<std::unique_ptr<CommandNode>> commandList();
    std::unique_ptr<IfCommandNode> ifCommand();
    std::unique_ptr<WhileCommandNode> whileCommand();
    std::unique_ptr<PrintCommandNode>  printCommand();
    std::unique_ptr<CommandNode> assignmentCommand();

    std::unique_ptr<ExpressionNode> primaryExpression();
    std::unique_ptr<ExpressionNode> postfixExpression();
    std::unique_ptr<ExpressionNode> unaryExpression();
    std::unique_ptr<ExpressionNode> multiplicationExpression();
    std::unique_ptr<ExpressionNode> additionDifferenceExpression();
    std::unique_ptr<ExpressionNode> comparisonExpression();
    std::unique_ptr<ExpressionNode> andExpression();
    std::unique_ptr<ExpressionNode> expression();
    std::vector<std::unique_ptr<ExpressionNode>> listExpression();




    public:
    Parser(const std::vector<token>& tokens) : tokens(tokens) {}
    std::unique_ptr<ProgramNode> parse();
    const SymbolTable& getSymbolTable() const { return symbolTable; }
};



#endif
