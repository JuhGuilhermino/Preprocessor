cmake_minimum_required(VERSION 3.10)
project(Preprocessor)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(preprocessor
    src/main.cpp
    src/Compiler.cpp
    src/CompileHelper.cpp
    src/ASTPrinter.cpp
    src/FileHandler.cpp
    src/lexer.cpp
    src/parser.cpp
    src/SymbolTable.cpp
    src/SemanticAnalyzer.cpp
)
