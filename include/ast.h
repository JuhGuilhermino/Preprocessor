#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>

struct TypeNode{
    std::string name;
    bool isArray = false;
};

struct VarDeclNode{
    TypeNode type;
    std::string name;
};

// Se usassemos só commandNode, iria vir só a parte do command node. 
// A parte específica não iria vir. Além disso, só sabemos qu eé um command ndoe, pode ser qualquer tipo
struct CommandNode{
    virtual ~CommandNode() = default;
};

struct ExpressionNode{
    virtual ~ExpressionNode() = default;
};


struct BlockCommandNode : CommandNode{
    std::vector<std::unique_ptr<CommandNode>> commands; 
};

struct IfCommandNode: CommandNode
{
    std::unique_ptr<ExpressionNode> condition;
    std::unique_ptr<CommandNode> thenCommand;
    std::unique_ptr<CommandNode> elseCommand;
};

struct WhileCommandNode : CommandNode{
    std::unique_ptr<ExpressionNode> condition;
    std::unique_ptr<CommandNode> while_block;
};

struct PrintCommandNode : CommandNode{
    std::unique_ptr<ExpressionNode> expression;
};


struct AssignmentCommandNode : CommandNode{
    std::string target;
    std::unique_ptr<ExpressionNode> value;
};

struct ArrayAssignmentCommandNode : CommandNode{
    std::string array_name;
    std::unique_ptr<ExpressionNode> index;
    std::unique_ptr<ExpressionNode> value;
};

struct IntegerExpressionNode : ExpressionNode
{
    int value;
};


struct BooleanExpressionNode : ExpressionNode
{
    bool value;
};

struct IdentifierExpressionNode : ExpressionNode // nome da variável para uso (Ex: num + 1)
{
    std::string name;
};

struct ThisExpressionNode : ExpressionNode
{};

struct BinaryExpressionNode : ExpressionNode
{
    std::unique_ptr<ExpressionNode> value1;
    std::string op;
    std::unique_ptr<ExpressionNode> value2;
};

struct UnaryExpressionNode : ExpressionNode
{
    std::unique_ptr<ExpressionNode> value;
    std::string op;
};

struct NewObjectExpressionNode : ExpressionNode
{
    std::string class_name;
};

struct NewArrayExpressionNode : ExpressionNode{
    std::unique_ptr<ExpressionNode> size;
};

struct ArrayAccessExpressionNode : ExpressionNode{
    std::unique_ptr<ExpressionNode> array;
    std::unique_ptr<ExpressionNode> index;
};

struct ArrayLengthExpressionNode : ExpressionNode{
    std::unique_ptr<ExpressionNode> array;
};

struct MethodCallExpressionNode : ExpressionNode{
    std::unique_ptr<ExpressionNode> object;
    std::string method_name;
    std::vector<std::unique_ptr<ExpressionNode>> arguments;
};

struct MethodNode{
    TypeNode returnType;
    std::string name;
    std::vector<VarDeclNode> parameters;
    std::vector<VarDeclNode> localVariables;
    std::vector<std::unique_ptr<CommandNode>> commands; // A lista guarda referências para comandos diferentes
    std::unique_ptr<ExpressionNode> returnExpression;
};

struct ClassNode{
    std::string name;
    std::string parentName = "";
    std::vector<VarDeclNode> variables;
    std::vector<MethodNode> methods;
};

struct MainClassNode{
    std::string name;
    std::string argsName;
    std::vector<std::unique_ptr<CommandNode>> commands;
};


struct ProgramNode{
    MainClassNode mainClass;
    std::vector<ClassNode> classes;
};

#endif
