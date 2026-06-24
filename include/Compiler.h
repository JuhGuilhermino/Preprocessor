#ifndef COMPILER_H
#define COMPILER_H

#include "lexer.h"
#include "parser.h"
#include "CompileHelper.h"
#include "FileHandler.h"


class Compiler{
    CompileHelper helper;
    Lexer lexer;
    Flags flags;
    FileHandler file_handler;
    std::unique_ptr<ProgramNode> ast;

    public:
    int compile(int argc, char* argv[]);

};




#endif
