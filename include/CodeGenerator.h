#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "ast.h"
#include "ThreeAddressCode.h"
#include "SymbolTable.h"
#include <string>

class CodeGenerator {
private:
    SymbolTable& symbolTable;
    std::string currentClass;
    std::string currentMethod;

    TACList generateCommand(CommandNode* command);

    TACList generateExpression(ExpressionNode* expr, Symbol*& resultOut);

public:
    CodeGenerator(SymbolTable& st) : symbolTable(st), currentClass(""), currentMethod("") {}

    TACList generate(ProgramNode& program);
    TACList generateClass(ClassNode& classNode);
    TACList generateMethod(MethodNode& methodNode);
};

#endif