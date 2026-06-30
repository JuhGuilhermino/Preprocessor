#include "ASTPrinter.h"

#include <iostream>

ASTPrinter::ASTPrinter(std::ostream& out) : out(out) {}

void ASTPrinter::print(const ProgramNode& program)
{
    line("Program");
    ++indentLevel;
    printMainClass(program.mainClass);
    for (const auto& classNode : program.classes)
    {
        printClass(classNode);
    }
    --indentLevel;
}

void ASTPrinter::line(const std::string& text)
{
    for (int i = 0; i < indentLevel; ++i)
    {
        out << "  ";
    }
    out << text << '\n';
}

std::string ASTPrinter::typeToString(const TypeNode& type) const
{
    return type.name + (type.isArray ? "[]" : "");
}

void ASTPrinter::printMainClass(const MainClassNode& mainClass)
{
    line("MainClass " + mainClass.name);
    ++indentLevel;
    line("args: " + mainClass.argsName);
    line("commands");
    ++indentLevel;
    printCommands(mainClass.commands);
    --indentLevel;
    --indentLevel;
}

void ASTPrinter::printClass(const ClassNode& classNode)
{
    std::string header = "Class " + classNode.name;
    if (!classNode.parentName.empty())
    {
        header += " extends " + classNode.parentName;
    }

    line(header);
    ++indentLevel;

    if (!classNode.variables.empty())
    {
        line("variables");
        ++indentLevel;
        for (const auto& variable : classNode.variables)
        {
            printVarDecl(variable);
        }
        --indentLevel;
    }

    if (!classNode.methods.empty())
    {
        line("methods");
        ++indentLevel;
        for (const auto& method : classNode.methods)
        {
            printMethod(method);
        }
        --indentLevel;
    }

    --indentLevel;
}

void ASTPrinter::printMethod(const MethodNode& method)
{
    line("Method " + method.name + " : " + typeToString(method.returnType));
    ++indentLevel;

    if (!method.parameters.empty())
    {
        line("parameters");
        ++indentLevel;
        for (const auto& parameter : method.parameters)
        {
            printVarDecl(parameter);
        }
        --indentLevel;
    }

    if (!method.localVariables.empty())
    {
        line("locals");
        ++indentLevel;
        for (const auto& variable : method.localVariables)
        {
            printVarDecl(variable);
        }
        --indentLevel;
    }

    if (!method.commands.empty())
    {
        line("commands");
        ++indentLevel;
        printCommands(method.commands);
        --indentLevel;
    }

    if (method.returnExpression)
    {
        line("return");
        ++indentLevel;
        printExpression(*method.returnExpression);
        --indentLevel;
    }

    --indentLevel;
}

void ASTPrinter::printVarDecl(const VarDeclNode& varDecl)
{
    line(typeToString(varDecl.type) + " " + varDecl.name);
}

void ASTPrinter::printCommands(const std::vector<std::unique_ptr<CommandNode>>& commands)
{
    for (const auto& command : commands)
    {
        if (command)
        {
            printCommand(*command);
        }
    }
}

void ASTPrinter::printCommand(const CommandNode& command)
{
    if (const auto* block = dynamic_cast<const BlockCommandNode*>(&command))
    {
        line("Block");
        ++indentLevel;
        printCommands(block->commands);
        --indentLevel;
    }
    else if (const auto* ifCommand = dynamic_cast<const IfCommandNode*>(&command))
    {
        line("If");
        ++indentLevel;
        line("condition");
        ++indentLevel;
        printExpression(*ifCommand->condition);
        --indentLevel;
        line("then");
        ++indentLevel;
        printCommand(*ifCommand->thenCommand);
        --indentLevel;
        line("else");
        ++indentLevel;
        printCommand(*ifCommand->elseCommand);
        --indentLevel;

        // Proteção contra nullptr
        if (ifCommand->elseCommand) {
            line("else");
            ++indentLevel;
            printCommand(*ifCommand->elseCommand);
            --indentLevel;
        }
        --indentLevel;
    }
    else if (const auto* whileCommand = dynamic_cast<const WhileCommandNode*>(&command))
    {
        line("While");
        ++indentLevel;
        line("condition");
        ++indentLevel;
        printExpression(*whileCommand->condition);
        --indentLevel;
        line("body");
        ++indentLevel;
        printCommand(*whileCommand->while_block);
        --indentLevel;
        --indentLevel;
    }
    else if (const auto* printCommand = dynamic_cast<const PrintCommandNode*>(&command))
    {
        line("Print");
        ++indentLevel;
        printExpression(*printCommand->expression);
        --indentLevel;
    }
    else if (const auto* assignment = dynamic_cast<const AssignmentCommandNode*>(&command))
    {
        line("Assign " + assignment->target);
        ++indentLevel;
        printExpression(*assignment->value);
        --indentLevel;
    }
    else if (const auto* arrayAssignment = dynamic_cast<const ArrayAssignmentCommandNode*>(&command))
    {
        line("ArrayAssign " + arrayAssignment->array_name);
        ++indentLevel;
        line("index");
        ++indentLevel;
        printExpression(*arrayAssignment->index);
        --indentLevel;
        line("value");
        ++indentLevel;
        printExpression(*arrayAssignment->value);
        --indentLevel;
        --indentLevel;
    }
    else
    {
        line("UnknownCommand");
    }
}

void ASTPrinter::printExpression(const ExpressionNode& expression)
{
    if (const auto* integer = dynamic_cast<const IntegerExpressionNode*>(&expression))
    {
        line("Integer " + std::to_string(integer->value));
    }
    else if (const auto* boolean = dynamic_cast<const BooleanExpressionNode*>(&expression))
    {
        line(std::string("Boolean ") + (boolean->value ? "true" : "false"));
    }
    else if (const auto* identifier = dynamic_cast<const IdentifierExpressionNode*>(&expression))
    {
        line("Identifier " + identifier->name);
    }
    else if (dynamic_cast<const ThisExpressionNode*>(&expression))
    {
        line("This");
    }
    else if (const auto* binary = dynamic_cast<const BinaryExpressionNode*>(&expression))
    {
        line("Binary " + binary->op);
        ++indentLevel;
        printExpression(*binary->value1);
        printExpression(*binary->value2);
        --indentLevel;
    }
    else if (const auto* unary = dynamic_cast<const UnaryExpressionNode*>(&expression))
    {
        line("Unary " + unary->op);
        ++indentLevel;
        printExpression(*unary->value);
        --indentLevel;
    }
    else if (const auto* newObject = dynamic_cast<const NewObjectExpressionNode*>(&expression))
    {
        line("NewObject " + newObject->class_name);
    }
    else if (const auto* newArray = dynamic_cast<const NewArrayExpressionNode*>(&expression))
    {
        line("NewArray int");
        ++indentLevel;
        line("size");
        ++indentLevel;
        printExpression(*newArray->size);
        --indentLevel;
        --indentLevel;
    }
    else if (const auto* arrayAccess = dynamic_cast<const ArrayAccessExpressionNode*>(&expression))
    {
        line("ArrayAccess");
        ++indentLevel;
        line("array");
        ++indentLevel;
        printExpression(*arrayAccess->array);
        --indentLevel;
        line("index");
        ++indentLevel;
        printExpression(*arrayAccess->index);
        --indentLevel;
        --indentLevel;
    }
    else if (const auto* arrayLength = dynamic_cast<const ArrayLengthExpressionNode*>(&expression))
    {
        line("ArrayLength");
        ++indentLevel;
        printExpression(*arrayLength->array);
        --indentLevel;
    }
    else if (const auto* call = dynamic_cast<const MethodCallExpressionNode*>(&expression))
    {
        line("MethodCall " + call->method_name);
        ++indentLevel;
        line("object");
        ++indentLevel;
        printExpression(*call->object);
        --indentLevel;
        if (!call->arguments.empty())
        {
            line("arguments");
            ++indentLevel;
            for (const auto& argument : call->arguments)
            {
                printExpression(*argument);
            }
            --indentLevel;
        }
        --indentLevel;
    }
    else
    {
        line("UnknownExpression");
    }
}
