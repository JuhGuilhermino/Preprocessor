#ifndef AST_PRINTER_H
#define AST_PRINTER_H

#include "ast.h"
#include <iosfwd>
#include <string>

class ASTPrinter {
public:
    explicit ASTPrinter(std::ostream& out);
    void print(const ProgramNode& program);

private:
    std::ostream& out;
    int indentLevel = 0;

    void line(const std::string& text);
    std::string typeToString(const TypeNode& type) const;

    void printMainClass(const MainClassNode& mainClass);
    void printClass(const ClassNode& classNode);
    void printMethod(const MethodNode& method);
    void printVarDecl(const VarDeclNode& varDecl);
    void printCommands(const std::vector<std::unique_ptr<CommandNode>>& commands);
    void printCommand(const CommandNode& command);
    void printExpression(const ExpressionNode& expression);
};

#endif
