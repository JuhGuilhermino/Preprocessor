#ifndef COMPILER_H
#define COMPILER_H

#include "lexer.h"
#include "parser.h"
#include "CompileHelper.h"
#include "FileHandler.h"
#include "SemanticAnalyzer.h"
#include "SymbolTable.h"
#include "CodeGenerator.h"
#include "ThreeAddressCode.h"


class Compiler{
    CompileHelper helper;
    Lexer lexer;
    Flags flags;
    FileHandler file_handler;
    std::unique_ptr<ProgramNode> ast;
    SymbolTable symbolTable;


    public:
    int compile(int argc, char* argv[]);

};

#endif
