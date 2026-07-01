#include "SemanticAnalyzer.h"
#include <iostream>

void SemanticAnalyzer::error(const std::string& message) {
        throw std::runtime_error("Erro Semântico: " + message);
    }

bool isPrimitive(const std::string& type) {
    return type == "int" || type == "boolean" || type == "int[]";
}

bool SemanticAnalyzer::isCompatible(const std::string& parent, const std::string& sub) {
    if (parent == sub) return true;
    
    std::string ancestor = symbolTable.getParentClass(sub);
    while (!ancestor.empty()) {
        if (ancestor == parent) return true;
        ancestor = symbolTable.getParentClass(ancestor);
    }
    return false;
}

void SemanticAnalyzer::analyze(ProgramNode& program) {
    currentClass = program.mainClass.name;
    currentMethod = "main";
    for (auto& cmd : program.mainClass.commands) {
        analyzeCommand(cmd.get());
    }
    
    for (auto& classNode : program.classes) {
        analyze(classNode);
    }
}

void SemanticAnalyzer::analyze(ClassNode& classNode) {
    currentClass = classNode.name;
    currentMethod = "";

    if (classNode.variables.empty() && classNode.methods.empty()) {
        error("A classe '" + classNode.name + "' não pode ser vazia.");
    }

    for (auto& method : classNode.methods) {
        analyze(method);
    }
}

void SemanticAnalyzer::analyze(MethodNode& methodNode) {
    currentMethod = methodNode.name;

    for (auto& cmd : methodNode.commands) {
        analyzeCommand(cmd.get());
    }

    std::string returnType = analyzeExpression(methodNode.returnExpression.get());
    std::string expectedType = methodNode.returnType.name + (methodNode.returnType.isArray ? "[]" : "");

    if (!isCompatible(expectedType, returnType)) {
        error("O tipo retornado '" + returnType + "' é incompatível com o tipo de retorno '" + expectedType + "' do método '" + currentMethod + "'.");
    }
}

void SemanticAnalyzer::analyzeCommand(CommandNode* command) {
    if (!command) return;

    if (auto block = dynamic_cast<BlockCommandNode*>(command)) {
        for (auto& cmd : block->commands) {
            analyzeCommand(cmd.get());
        }
    }
    else if (auto ifCmd = dynamic_cast<IfCommandNode*>(command)) {
        std::string condType = analyzeExpression(ifCmd->condition.get());
        if (condType != "boolean") {
            error("A condição do 'if' deve ser do tipo 'boolean', mas retornou '" + condType + "'.");
        }
        analyzeCommand(ifCmd->thenCommand.get());
        if (ifCmd->elseCommand) {
            analyzeCommand(ifCmd->elseCommand.get());
        }
    }
    else if (auto whileCmd = dynamic_cast<WhileCommandNode*>(command)) {
        std::string condType = analyzeExpression(whileCmd->condition.get());
        if (condType != "boolean") {
            error("A condição do 'while' deve ser do tipo 'boolean', mas retornou '" + condType + "'.");
        }
        analyzeCommand(whileCmd->while_block.get());
    }
    else if (auto printCmd = dynamic_cast<PrintCommandNode*>(command)) {
        std::string exprType = analyzeExpression(printCmd->expression.get());
        if (exprType != "int" && exprType != "boolean") {
            error("O comando 'System.out.println' só aceita tipos primitivos (int ou boolean).");
        }
    }
    else if (auto assignCmd = dynamic_cast<AssignmentCommandNode*>(command)) {
        std::string targetType = symbolTable.lookupVariableType(assignCmd->target, currentClass, currentMethod);
        if (targetType.empty()) {
            error("Variável '" + assignCmd->target + "' não foi declarada neste escopo.");
        }

        std::string exprType = analyzeExpression(assignCmd->value.get());
        
        if (!isCompatible(targetType, exprType)) { 
            error("Tipos incompatíveis na atribuição: não é possível atribuir '" + exprType + "' a '" + targetType + "'.");
        }
    }
    else if (auto arrayAssignCmd = dynamic_cast<ArrayAssignmentCommandNode*>(command)) {
        std::string arrayType = symbolTable.lookupVariableType(arrayAssignCmd->array_name, currentClass, currentMethod);
        if (arrayType != "int[]") {
            error("A variável '" + arrayAssignCmd->array_name + "' não é um vetor.");
        }

        std::string indexType = analyzeExpression(arrayAssignCmd->index.get());
        if (indexType != "int") {
            error("O índice do vetor deve ser um 'int', mas retornou '" + indexType + "'.");
        }

        std::string exprType = analyzeExpression(arrayAssignCmd->value.get());
        if (exprType != "int") {
            error("Não é possível atribuir '" + exprType + "' a um elemento de vetor 'int[]'.");
        }
    }
}

std::string SemanticAnalyzer::analyzeExpression(ExpressionNode* expr) {
    if (!expr) return "";

    if (dynamic_cast<IntegerExpressionNode*>(expr)) {
        return "int";
    }

    if (dynamic_cast<BooleanExpressionNode*>(expr)) {
        return "boolean";
    }

    if (dynamic_cast<ThisExpressionNode*>(expr)) {
        return currentClass;
    }
   
    if (auto idExpr = dynamic_cast<IdentifierExpressionNode*>(expr)) {
        std::string type = symbolTable.lookupVariableType(idExpr->name, currentClass, currentMethod);
        if (type.empty()) {
            error("Identificador '" + idExpr->name + "' não declarado.");
        }
        return type;
    }

    if (auto binExpr = dynamic_cast<BinaryExpressionNode*>(expr)) {
        std::string t1 = analyzeExpression(binExpr->value1.get());
        std::string t2 = analyzeExpression(binExpr->value2.get());

        if (binExpr->op == "+" || binExpr->op == "-") {
            if (t1 == "int" && t2 == "int") return "int";
            error("Operador '" + binExpr->op + "' exige operandos do tipo 'int'.");
        }
        else if (binExpr->op == "<") {
            if (t1 == "int" && t2 == "int") return "boolean";
            error("Operador '<' exige operandos do tipo 'int'.");
        }
        else if (binExpr->op == "&&") {
            if (t1 == "boolean" && t2 == "boolean") return "boolean";
            error("Operador '&&' exige operandos do tipo 'boolean'.");
        }
    }

    if (auto unaryExpr = dynamic_cast<UnaryExpressionNode*>(expr)) {
        std::string t = analyzeExpression(unaryExpr->value.get());
        if (unaryExpr->op == "!" && t == "boolean") return "boolean";
        error("Operador '!' exige operando do tipo 'boolean'.");
    }

    if (auto lengthExpr = dynamic_cast<ArrayLengthExpressionNode*>(expr)) {
        std::string t = analyzeExpression(lengthExpr->array.get());
        if (t == "int[]") return "int";
        error("O atributo 'length' só pode ser usado em vetores.");
    }

    if (auto accessExpr = dynamic_cast<ArrayAccessExpressionNode*>(expr)) {
        std::string arrayT = analyzeExpression(accessExpr->array.get());
        std::string indexT = analyzeExpression(accessExpr->index.get());
        if (arrayT == "int[]" && indexT == "int") return "int";
        error("Acesso a vetor inválido. Certifique-se de que o objeto é 'int[]' e o índice é 'int'.");
    }

    if (auto newObjExpr = dynamic_cast<NewObjectExpressionNode*>(expr)) {
        if (!symbolTable.classExists(newObjExpr->class_name)) {
            error("Classe '" + newObjExpr->class_name + "' usada em 'new' não existe.");
        }
        return newObjExpr->class_name;
    }

    if (auto newArrayExpr = dynamic_cast<NewArrayExpressionNode*>(expr)) {
        std::string sizeT = analyzeExpression(newArrayExpr->size.get());
        if (sizeT != "int") {
            error("O tamanho da alocação de um vetor deve ser 'int'.");
        }
        return "int[]";
    }
  
    if (auto callExpr = dynamic_cast<MethodCallExpressionNode*>(expr)) {
        std::string objType = analyzeExpression(callExpr->object.get());
        
        std::string methodRetType = symbolTable.getMethodReturnType(objType, callExpr->method_name);
        if (methodRetType.empty()) {
            error("O método '" + callExpr->method_name + "' não existe na classe '" + objType + "'.");
        }

        auto expectedParams = symbolTable.getMethodParameters(objType, callExpr->method_name);
        if (callExpr->arguments.size() != expectedParams.size()) {
            error("Quantidade de argumentos inválida para o método '" + callExpr->method_name + "'.");
        }

        for (size_t i = 0; i < callExpr->arguments.size(); ++i) {
            std::string argType = analyzeExpression(callExpr->arguments[i].get());
            if (!isCompatible(expectedParams[i], argType)) {
                error("Argumento " + std::to_string(i+1) + " do método '" + callExpr->method_name + "' esperava '" + expectedParams[i] + "', mas recebeu '" + argType + "'.");
            }
        }

        return methodRetType;
    }

    return "";
}