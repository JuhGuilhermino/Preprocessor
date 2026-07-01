#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "parser.h"
#include <string>
#include <stdexcept>
#include <memory>

class SemanticAnalyzer {
private:
    SymbolTable& symbolTable;
    std::string currentClass;
    std::string currentMethod;

  
    void error(const std::string& message);

    bool isPrimitive(const std::string& type);

    bool isCompatible(const std::string& parent, const std::string& sub);

public:
    SemanticAnalyzer(SymbolTable& st) : symbolTable(st), currentClass(""), currentMethod("") {}

    void analyze(ProgramNode& program);
    void analyze(ClassNode& classNode);
    void analyze(MethodNode& methodNode);
    
    void analyzeCommand(CommandNode* command);
    
    std::string analyzeExpression(ExpressionNode* expr);
};

#endif