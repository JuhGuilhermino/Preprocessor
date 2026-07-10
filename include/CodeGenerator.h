#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "ast.h"
#include "ThreeAddressCode.h"
#include "SymbolTable.h"
#include <memory>
#include <string>
#include <vector>

class CodeGenerator {
private:
    SymbolTable& symbolTable;
    std::string currentClass;
    std::string currentMethod;
    std::vector<std::unique_ptr<Symbol>> ownedSymbols;

    TACList generateCommand(CommandNode* command);

    TACList generateExpression(ExpressionNode* expr, Symbol*& resultOut);
    Symbol* makeSymbol(const std::string& name, const std::string& type = "int");
    Symbol* copySymbol(const Symbol& symbol);

public:
    CodeGenerator(SymbolTable& st) : symbolTable(st), currentClass(""), currentMethod("") {}

    TACList generate(ProgramNode& program);
    TACList generateClass(ClassNode& classNode);
    TACList generateMethod(MethodNode& methodNode);
};

#endif
