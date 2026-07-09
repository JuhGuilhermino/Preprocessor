#include "CodeGenerator.h"
#include <stdexcept>

TACList CodeGenerator::generate(ProgramNode& program) {
    TACList programCode;

    currentClass = program.mainClass.name;
    currentMethod = "main";
    for (auto& cmd : program.mainClass.commands) {
        TACList tempCmd = generateCommand(cmd.get());
        programCode.concatenate(tempCmd);
    }

    for (auto& classNode : program.classes) {
        TACList classCode = generateClass(classNode);
        programCode.concatenate(classCode);
    }

    return programCode;
}

TACList CodeGenerator::generateClass(ClassNode& classNode) {
    currentClass = classNode.name;
    TACList classCode;

    for (auto& method : classNode.methods) {
        TACList methodCode = generateMethod(method);
        classCode.concatenate(methodCode);
    }
    return classCode;
}

TACList CodeGenerator::generateMethod(MethodNode& methodNode) {
    currentMethod = methodNode.name;
    TACList methodCode;

    Symbol* methodLabel = symbolTable.newLabel();
    methodLabel->name = currentClass + "_" + currentMethod;
    methodCode.appendInstruction(std::make_unique<Instruction>(LABEL, nullptr, nullptr, methodLabel));

    for (auto& cmd : methodNode.commands) {
        TACList tempCmd = generateCommand(cmd.get());
        methodCode.concatenate(tempCmd);
    }

    if (methodNode.returnExpression) {
        Symbol* retResult = nullptr;
        TACList retExprCode = generateExpression(methodNode.returnExpression.get(), retResult);
        methodCode.concatenate(retExprCode);
        methodCode.appendInstruction(std::make_unique<Instruction>(RETURN, retResult, nullptr, nullptr));
    }

    return methodCode;
}

TACList CodeGenerator::generateCommand(CommandNode* command) {
    TACList cmdCode;
    if (!command) return cmdCode;

    if (auto block = dynamic_cast<BlockCommandNode*>(command)) {
        for (auto& cmd : block->commands) {
            TACList tempCmd = generateCommand(cmd.get());
            cmdCode.concatenate(tempCmd);
        }
    }
    else if (auto assignCmd = dynamic_cast<AssignmentCommandNode*>(command)) {
        Symbol* valResult = nullptr;
        TACList valCode = generateExpression(assignCmd->value.get(), valResult);
        cmdCode.concatenate(valCode);

        auto targetOpt = symbolTable.lookupVariable(assignCmd->target, currentClass, currentMethod);
        if (targetOpt.has_value()) {
            Symbol* targetSym = new Symbol(targetOpt.value());
            cmdCode.appendInstruction(std::make_unique<Instruction>(ASSIGN, valResult, nullptr, targetSym));
        }
    }
    else if (auto printCmd = dynamic_cast<PrintCommandNode*>(command)) {
        Symbol* exprResult = nullptr;
        TACList exprCode = generateExpression(printCmd->expression.get(), exprResult);
        cmdCode.concatenate(exprCode);

        cmdCode.appendInstruction(std::make_unique<Instruction>(PRINT, exprResult, nullptr, nullptr));
    }
    else if (auto ifCmd = dynamic_cast<IfCommandNode*>(command)) {
        Symbol* condResult = nullptr;
        TACList condCode = generateExpression(ifCmd->condition.get(), condResult);
        cmdCode.concatenate(condCode);

        Symbol* elseLabel = symbolTable.newLabel();
        Symbol* endLabel = symbolTable.newLabel();

        cmdCode.appendInstruction(std::make_unique<Instruction>(IF_FALSE, condResult, nullptr, elseLabel));

        TACList thenCmdCode = generateCommand(ifCmd->thenCommand.get());
        cmdCode.concatenate(thenCmdCode);
        
        cmdCode.appendInstruction(std::make_unique<Instruction>(GOTO, nullptr, nullptr, endLabel));
        cmdCode.appendInstruction(std::make_unique<Instruction>(LABEL, nullptr, nullptr, elseLabel));
        
        if (ifCmd->elseCommand) {
            TACList elseCmdCode = generateCommand(ifCmd->elseCommand.get());
            cmdCode.concatenate(elseCmdCode);
        }
        
        cmdCode.appendInstruction(std::make_unique<Instruction>(LABEL, nullptr, nullptr, endLabel));
    }
    else if (auto whileCmd = dynamic_cast<WhileCommandNode*>(command)) {
        Symbol* startLabel = symbolTable.newLabel();
        Symbol* endLabel = symbolTable.newLabel();

        cmdCode.appendInstruction(std::make_unique<Instruction>(LABEL, nullptr, nullptr, startLabel));

        Symbol* condResult = nullptr;
        TACList condCode = generateExpression(whileCmd->condition.get(), condResult);
        cmdCode.concatenate(condCode);

        cmdCode.appendInstruction(std::make_unique<Instruction>(IF_FALSE, condResult, nullptr, endLabel));

        TACList whileBodyCode = generateCommand(whileCmd->while_block.get());
        cmdCode.concatenate(whileBodyCode);

        cmdCode.appendInstruction(std::make_unique<Instruction>(GOTO, nullptr, nullptr, startLabel));
        cmdCode.appendInstruction(std::make_unique<Instruction>(LABEL, nullptr, nullptr, endLabel));
    }

    return cmdCode;
}


TACList CodeGenerator::generateExpression(ExpressionNode* expr, Symbol*& resultOut) {
    TACList exprCode;
    if (!expr) return exprCode;

    if (auto integerNode = dynamic_cast<IntegerExpressionNode*>(expr)) {
        resultOut = symbolTable.newTemp(currentClass, currentMethod);
        resultOut->name = std::to_string(integerNode->value);
    }
    else if (auto booleanNode = dynamic_cast<BooleanExpressionNode*>(expr)) {
        resultOut = symbolTable.newTemp(currentClass, currentMethod);
        resultOut->name = booleanNode->value ? "true" : "false";
    }
    else if (auto idNode = dynamic_cast<IdentifierExpressionNode*>(expr)) {
        auto varOpt = symbolTable.lookupVariable(idNode->name, currentClass, currentMethod);
        if (varOpt.has_value()) {
            resultOut = new Symbol(varOpt.value());
        }
    }
    else if (auto binNode = dynamic_cast<BinaryExpressionNode*>(expr)) {
        Symbol* leftRes = nullptr;
        Symbol* rightRes = nullptr;

        TACList leftCode = generateExpression(binNode->value1.get(), leftRes);
        TACList rightCode = generateExpression(binNode->value2.get(), rightRes);

        exprCode.concatenate(leftCode);
        exprCode.concatenate(rightCode);

        resultOut = symbolTable.newTemp(currentClass, currentMethod);

        opcode opCode = ADD;
        if (binNode->op == "-") opCode = SUB;
        else if (binNode->op == "*") opCode = MUL;
        else if (binNode->op == "&&") opCode = AND;

        exprCode.appendInstruction(std::make_unique<Instruction>(opCode, leftRes, rightRes, resultOut));
    }
    else if (auto unaryNode = dynamic_cast<UnaryExpressionNode*>(expr)) {
        Symbol* valRes = nullptr;
        TACList valCode = generateExpression(unaryNode->value.get(), valRes);
        exprCode.concatenate(valCode);

        resultOut = symbolTable.newTemp(currentClass, currentMethod);
        if (unaryNode->op == "!") {
            exprCode.appendInstruction(std::make_unique<Instruction>(NOT, valRes, nullptr, resultOut));
        }
    }

    return exprCode;
}