#include "CodeGenerator.h"
#include <stdexcept>

Symbol *CodeGenerator::makeSymbol(const std::string &name, const std::string &type)
{
    std::string scope = currentMethod.empty()
                            ? "class_" + currentClass
                            : "method_" + currentClass + "_" + currentMethod;

    auto symbol = std::make_unique<Symbol>(Symbol{name, type, symbol_category_e::VARIABLE, scope});
    Symbol *ptr = symbol.get();
    ownedSymbols.push_back(std::move(symbol));
    return ptr;
}

Symbol *CodeGenerator::copySymbol(const Symbol &symbol)
{
    auto copy = std::make_unique<Symbol>(symbol);
    Symbol *ptr = copy.get();
    ownedSymbols.push_back(std::move(copy));
    return ptr;
}

TACList CodeGenerator::generate(ProgramNode &program)
{
    TACList programCode;

    currentClass = program.mainClass.name;
    currentMethod = "main";
    for (auto &cmd : program.mainClass.commands)
    {
        TACList tempCmd = generateCommand(cmd.get());
        programCode.concatenate(tempCmd);
    }

    for (auto &classNode : program.classes)
    {
        TACList classCode = generateClass(classNode);
        programCode.concatenate(classCode);
    }

    return programCode;
}

TACList CodeGenerator::generateClass(ClassNode &classNode)
{
    currentClass = classNode.name;
    TACList classCode;

    for (auto &method : classNode.methods)
    {
        TACList methodCode = generateMethod(method);
        classCode.concatenate(methodCode);
    }
    return classCode;
}

TACList CodeGenerator::generateMethod(MethodNode &methodNode)
{
    currentMethod = methodNode.name;
    TACList methodCode;

    Symbol *methodLabel = symbolTable.newLabel();
    methodLabel->name = currentClass + "_" + currentMethod;
    methodCode.appendInstruction(std::make_unique<Instruction>(LABEL, nullptr, nullptr, methodLabel));

    for (auto &cmd : methodNode.commands)
    {
        TACList tempCmd = generateCommand(cmd.get());
        methodCode.concatenate(tempCmd);
    }

    if (methodNode.returnExpression)
    {
        Symbol *retResult = nullptr;
        TACList retExprCode = generateExpression(methodNode.returnExpression.get(), retResult);
        methodCode.concatenate(retExprCode);
        methodCode.appendInstruction(std::make_unique<Instruction>(RETURN, retResult, nullptr, nullptr));
    }

    return methodCode;
}

TACList CodeGenerator::generateCommand(CommandNode *command)
{
    TACList cmdCode;
    if (!command)
        return cmdCode;

    if (auto block = dynamic_cast<BlockCommandNode *>(command))
    {
        for (auto &cmd : block->commands)
        {
            TACList tempCmd = generateCommand(cmd.get());
            cmdCode.concatenate(tempCmd);
        }
    }
    else if (auto assignCmd = dynamic_cast<AssignmentCommandNode *>(command))
    {
        Symbol *valResult = nullptr;
        TACList valCode = generateExpression(assignCmd->value.get(), valResult);
        cmdCode.concatenate(valCode);

        auto targetOpt = symbolTable.lookupVariable(assignCmd->target, currentClass, currentMethod);
        if (targetOpt.has_value())
        {
            Symbol *targetSym = copySymbol(targetOpt.value());
            cmdCode.appendInstruction(std::make_unique<Instruction>(ASSIGN, valResult, nullptr, targetSym));
        }
        else
        {
            throw std::runtime_error("Variável '" + assignCmd->target + "' não encontrada para atribuição.");
        }
    }
    else if (auto arrayAssignCmd = dynamic_cast<ArrayAssignmentCommandNode *>(command))
    {
        auto arrayOpt = symbolTable.lookupVariable(arrayAssignCmd->array_name, currentClass, currentMethod);
        if (!arrayOpt.has_value())
        {
            throw std::runtime_error("Vetor '" + arrayAssignCmd->array_name + "' não encontrado para atribuição.");
        }

        Symbol *indexResult = nullptr;
        TACList indexCode = generateExpression(arrayAssignCmd->index.get(), indexResult);
        cmdCode.concatenate(indexCode);

        Symbol *valResult = nullptr;
        TACList valCode = generateExpression(arrayAssignCmd->value.get(), valResult);
        cmdCode.concatenate(valCode);

        cmdCode.appendInstruction(std::make_unique<Instruction>(
            ARRAY_STORE, indexResult, valResult, copySymbol(arrayOpt.value())));
    }
    else if (auto printCmd = dynamic_cast<PrintCommandNode *>(command))
    {
        Symbol *exprResult = nullptr;
        TACList exprCode = generateExpression(printCmd->expression.get(), exprResult);
        cmdCode.concatenate(exprCode);

        cmdCode.appendInstruction(std::make_unique<Instruction>(PRINT, exprResult, nullptr, nullptr));
    }
    else if (auto ifCmd = dynamic_cast<IfCommandNode *>(command))
    {
        Symbol *condResult = nullptr;
        TACList condCode = generateExpression(ifCmd->condition.get(), condResult);
        cmdCode.concatenate(condCode);

        Symbol *elseLabel = symbolTable.newLabel();
        Symbol *endLabel = symbolTable.newLabel();

        cmdCode.appendInstruction(std::make_unique<Instruction>(IF_FALSE, condResult, nullptr, elseLabel));

        TACList thenCmdCode = generateCommand(ifCmd->thenCommand.get());
        cmdCode.concatenate(thenCmdCode);

        cmdCode.appendInstruction(std::make_unique<Instruction>(GOTO, nullptr, nullptr, endLabel));
        cmdCode.appendInstruction(std::make_unique<Instruction>(LABEL, nullptr, nullptr, elseLabel));

        if (ifCmd->elseCommand)
        {
            TACList elseCmdCode = generateCommand(ifCmd->elseCommand.get());
            cmdCode.concatenate(elseCmdCode);
        }

        cmdCode.appendInstruction(std::make_unique<Instruction>(LABEL, nullptr, nullptr, endLabel));
    }
    else if (auto whileCmd = dynamic_cast<WhileCommandNode *>(command))
    {
        Symbol *startLabel = symbolTable.newLabel();
        Symbol *endLabel = symbolTable.newLabel();

        cmdCode.appendInstruction(std::make_unique<Instruction>(LABEL, nullptr, nullptr, startLabel));

        Symbol *condResult = nullptr;
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

TACList CodeGenerator::generateExpression(ExpressionNode *expr, Symbol *&resultOut)
{
    TACList exprCode;
    if (!expr)
        return exprCode;

    if (auto integerNode = dynamic_cast<IntegerExpressionNode *>(expr))
    {
        resultOut = makeSymbol(std::to_string(integerNode->value));
    }
    else if (auto booleanNode = dynamic_cast<BooleanExpressionNode *>(expr))
    {
        resultOut = makeSymbol(booleanNode->value ? "true" : "false", "boolean");
    }
    else if (auto idNode = dynamic_cast<IdentifierExpressionNode *>(expr))
    {
        auto varOpt = symbolTable.lookupVariable(idNode->name, currentClass, currentMethod);
        if (varOpt.has_value())
        {
            resultOut = copySymbol(varOpt.value());
        }
        else
        {
            throw std::runtime_error("Identificador '" + idNode->name + "' não encontrado.");
        }
    }
    else if (dynamic_cast<ThisExpressionNode *>(expr))
    {
        resultOut = makeSymbol("this", currentClass);
    }
    else if (auto binNode = dynamic_cast<BinaryExpressionNode *>(expr))
    {
        Symbol *leftRes = nullptr;
        Symbol *rightRes = nullptr;

        TACList leftCode = generateExpression(binNode->value1.get(), leftRes);
        TACList rightCode = generateExpression(binNode->value2.get(), rightRes);

        exprCode.concatenate(leftCode);
        exprCode.concatenate(rightCode);

        resultOut = symbolTable.newTemp(currentClass, currentMethod);

        opcode opCode = ADD;
        if (binNode->op == "-")
            opCode = SUB;
        else if (binNode->op == "*")
            opCode = MUL;
        else if (binNode->op == "<")
            opCode = LT;
        else if (binNode->op == "&&")
            opCode = AND;
        else if (binNode->op != "+")
        {
            throw std::runtime_error("Operador binário não suportado no 3AC: '" + binNode->op + "'.");
        }

        exprCode.appendInstruction(std::make_unique<Instruction>(opCode, leftRes, rightRes, resultOut));
    }
    else if (auto unaryNode = dynamic_cast<UnaryExpressionNode *>(expr))
    {
        Symbol *valRes = nullptr;
        TACList valCode = generateExpression(unaryNode->value.get(), valRes);
        exprCode.concatenate(valCode);

        resultOut = symbolTable.newTemp(currentClass, currentMethod);
        if (unaryNode->op == "!")
        {
            exprCode.appendInstruction(std::make_unique<Instruction>(NOT, valRes, nullptr, resultOut));
        }
        else
        {
            throw std::runtime_error("Operador unário não suportado no 3AC: '" + unaryNode->op + "'.");
        }
    }
    else if (auto newObjectNode = dynamic_cast<NewObjectExpressionNode *>(expr))
    {
        resultOut = symbolTable.newTemp(currentClass, currentMethod);
        exprCode.appendInstruction(std::make_unique<Instruction>(
            NEW_OBJECT, makeSymbol(newObjectNode->class_name, newObjectNode->class_name), nullptr, resultOut));
    }
    else if (auto newArrayNode = dynamic_cast<NewArrayExpressionNode *>(expr))
    {
        Symbol *sizeResult = nullptr;
        TACList sizeCode = generateExpression(newArrayNode->size.get(), sizeResult);
        exprCode.concatenate(sizeCode);

        resultOut = symbolTable.newTemp(currentClass, currentMethod);
        exprCode.appendInstruction(std::make_unique<Instruction>(NEW_ARRAY, sizeResult, nullptr, resultOut));
    }
    else if (auto arrayAccessNode = dynamic_cast<ArrayAccessExpressionNode *>(expr))
    {
        Symbol *arrayResult = nullptr;
        TACList arrayCode = generateExpression(arrayAccessNode->array.get(), arrayResult);
        exprCode.concatenate(arrayCode);

        Symbol *indexResult = nullptr;
        TACList indexCode = generateExpression(arrayAccessNode->index.get(), indexResult);
        exprCode.concatenate(indexCode);

        resultOut = symbolTable.newTemp(currentClass, currentMethod);
        exprCode.appendInstruction(std::make_unique<Instruction>(
            ARRAY_LOAD, arrayResult, indexResult, resultOut));
    }
    else if (auto arrayLengthNode = dynamic_cast<ArrayLengthExpressionNode *>(expr))
    {
        Symbol *arrayResult = nullptr;
        TACList arrayCode = generateExpression(arrayLengthNode->array.get(), arrayResult);
        exprCode.concatenate(arrayCode);

        resultOut = symbolTable.newTemp(currentClass, currentMethod);
        exprCode.appendInstruction(std::make_unique<Instruction>(
            ARRAY_LENGTH, arrayResult, nullptr, resultOut));
    }
    else if (auto callNode = dynamic_cast<MethodCallExpressionNode *>(expr))
    {
        Symbol *objectResult = nullptr;
        TACList objectCode = generateExpression(callNode->object.get(), objectResult);
        exprCode.concatenate(objectCode);

        for (auto &argument : callNode->arguments)
        {
            Symbol *argResult = nullptr;
            TACList argCode = generateExpression(argument.get(), argResult);
            exprCode.concatenate(argCode);
            exprCode.appendInstruction(std::make_unique<Instruction>(PARAM, argResult, nullptr, nullptr));
        }

        resultOut = symbolTable.newTemp(currentClass, currentMethod);
        std::string callName = objectResult->name + "." + callNode->method_name;
        exprCode.appendInstruction(std::make_unique<Instruction>(
            CALL, makeSymbol(callName), makeSymbol(std::to_string(callNode->arguments.size())), resultOut));
    }
    else
    {
        throw std::runtime_error("Expressão não suportada na geração de 3AC.");
    }

    return exprCode;
}
