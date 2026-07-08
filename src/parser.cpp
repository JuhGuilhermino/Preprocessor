#include "parser.h"

token Parser::peek()
{
    if (tokens.empty())
    {
        return {token_types_e::EOF_TOKEN, "", 0, 0};
    }

    if (current >= tokens.size())
    {
        return tokens.back();
    }

    return tokens[current];
}

token Parser::peekSpecific(int i)
{
    if (tokens.empty())
    {
        return {token_types_e::EOF_TOKEN, "", 0, 0};
    }

    if (i >= tokens.size())
    {
        return tokens.back();
    }

    return tokens[i];
}

token Parser::previous()
{
    if (tokens.empty())
    {
        return {token_types_e::EOF_TOKEN, "", 0, 0};
    }

    if (current == 0)
    {
        return tokens.front();
    }

    return tokens[current - 1];
}

token Parser::advance()
{
    if (!finished())
    {
        current++;
    }

    return previous();
}

bool Parser::finished()
{
    return peek().type == token_types_e::EOF_TOKEN;
}

bool Parser::check(token_types_e type)
{
    return peek().type == type;
}

bool Parser::checkValue(const std::string &lex)
{
    return peek().value == lex;
}

bool Parser::checkNext(token_types_e type)
{
    auto next = current + 1;
    return peekSpecific(next).type == type;
}

bool Parser::checkNextValue(const std::string &lex)
{
    auto next = current + 1;
    return peekSpecific(next).value == lex;
}

bool Parser::match(token_types_e type)
{
    if (check(type))
    {
        advance();
        return true;
    }

    return false;
}

bool Parser::matchValue(const std::string &value)
{
    if (checkValue(value))
    {
        advance();
        return true;
    }

    return false;
}

token Parser::consume(token_types_e type, std::string error)
{
    if (check(type))
    {
        return advance();
    }

    throw std::runtime_error(error);
}

token Parser::consumeValue(const std::string &value, std::string error)
{
    if (checkValue(value))
    {
        return advance();
    }
    throw std::runtime_error(error);
}
std::unique_ptr<BlockCommandNode> Parser::block()
{
    auto block = std::make_unique<BlockCommandNode>();
    consumeValue("{", "Esperado '{'");

    while (!checkValue("}") && !finished())
    {
        block->commands.push_back(commands());
    }

    consumeValue("}", "Esperado '}'");
    return block;
}

std::unique_ptr<IfCommandNode> Parser::ifCommand()
{
    auto ifNode = std::make_unique<IfCommandNode>();
    consumeValue("if", "Esperado 'if'");
    consumeValue("(", "Esperado '('");
    ifNode->condition = expression();
    consumeValue(")", "Esperado ')'");

    ifNode->thenCommand = commands();

    if (matchValue("else"))
    {
        if (checkValue("if"))
        {
            ifNode->elseCommand = ifCommand();
        }
        else
        {
            ifNode->elseCommand = commands(); 
        }
    }
    else
    {
        ifNode->elseCommand = nullptr;
    }

    return ifNode;
}

std::unique_ptr<WhileCommandNode> Parser::whileCommand()
{
    auto whileNode = std::make_unique<WhileCommandNode>();

    consumeValue("while", "Esperado 'while'");
    consumeValue("(", "Esperado '('");
    whileNode->condition = expression();
    consumeValue(")", "Esperado ')'");
    whileNode->while_block = commands();
    return whileNode;
}

std::unique_ptr<PrintCommandNode> Parser::printCommand()
{
    auto node = std::make_unique<PrintCommandNode>();
    consumeValue("System", "Esperado 'System'");
    consumeValue(".", "Esperado '.'");
    consumeValue("out", "Esperado 'out'");
    consumeValue(".", "Esperado '.'");
    consumeValue("println", "Esperado 'println'");
    consumeValue("(", "Esperado '('");
    node->expression = expression();
    consumeValue(")", "Esperado ')'");
    consumeValue(";", "Esperado ';'");
    return node;
}

std::unique_ptr<CommandNode> Parser::assignmentCommand()
{
    auto identifier = consume(token_types_e::IDENTIFIER, "Esperado identificador");

    if (checkValue("["))
    {
        auto node = std::make_unique<ArrayAssignmentCommandNode>();
        node->array_name = identifier.value;
        consumeValue("[", "Esperado '['");
        node->index = expression();
        consumeValue("]", "Esperado ']'");
        consumeValue("=", "Esperado '='");
        node->value = expression();
        consumeValue(";", "Esperado ';'");
        return node;
    }
    else
    {
        auto node = std::make_unique<AssignmentCommandNode>();
        node->target = identifier.value;
        consumeValue("=", "Esperado '='");
        node->value = expression();
        consumeValue(";", "Esperado ';'");
        return node;
    }
}

std::unique_ptr<CommandNode> Parser::commands()
{
    if (checkValue("{"))
    {
        return block();
    }
    else if (checkValue("if"))
    {
        return ifCommand();
    }
    else if (checkValue("while"))
    {
        return whileCommand();
    }
    else if (checkValue("System"))
    {
        return printCommand();
    }
    else if (check(token_types_e::IDENTIFIER))
    {
        return assignmentCommand();
    }
    else
    {
        throw std::runtime_error("Esperado comando");
    }
}

std::vector<std::unique_ptr<CommandNode>> Parser::commandList()
{
    std::vector<std::unique_ptr<CommandNode>> commandNodes;
    while (checkValue("{") || checkValue("if") || checkValue("while") ||
           checkValue("System") || check(token_types_e::IDENTIFIER))
    {
        commandNodes.push_back(commands());
    }
    return commandNodes;
}

void Parser::methodBlock(MethodNode &node)
{
    consumeValue("{", "Esperado '{'");
    node.localVariables = multipleVarsDeclarations();

    // Add cada variável local no escopo do método na tablea de simbolos
    for (const auto& var : node.localVariables) {
        std::string varTypeStr = var.type.name + (var.type.isArray ? "[]" : "");
        symbolTable.addSymbol(var.name, varTypeStr, symbol_category_e::VARIABLE, "method_" + currentClassName + "_" + currentMethodName);
    }

    node.commands = commandList();
    consumeValue("return", "Esperado 'return'");
    node.returnExpression = expression();
    consumeValue(";", "Esperado ';'");
    consumeValue("}", "Esperado '}'");
}

std::vector<VarDeclNode> Parser::args()
{
    std::vector<VarDeclNode> vars;
    VarDeclNode var;
    var.type = type();
    var.name = consume(token_types_e::IDENTIFIER, "Esperado nome do argumento").value;
    vars.push_back(var);
    while (checkValue(","))
    {
        VarDeclNode var;
        consumeValue(",", "Esperado ','");
        var.type = type();
        var.name = consume(token_types_e::IDENTIFIER, "Esperado nome do argumento").value;
        vars.push_back(var);
    }

    return vars;
}

MethodNode Parser::methodDeclaration()
{
    MethodNode method;
    consumeValue("public", "Esperado 'public'");
    method.returnType = type();
    method.name = consume(token_types_e::IDENTIFIER, "Esperado identificador do método").value;
    
    // Add o método da classe atual
    currentMethodName = method.name;
    std::string retTypeStr = method.returnType.name + (method.returnType.isArray ? "[]" : "");
    symbolTable.addSymbol(method.name, retTypeStr, symbol_category_e::METHOD, "class_" + currentClassName);

    consumeValue("(", "Esperado '('");

    if (!checkValue(")"))
    {
        method.parameters = args();
        std::vector<std::string> parameterTypes;
        // Add cada parâmetro na tabela de simbolos
        for (const auto& param : method.parameters) {
            std::string paramTypeStr = param.type.name + (param.type.isArray ? "[]" : "");
            parameterTypes.push_back(paramTypeStr);
            symbolTable.addSymbol(param.name, paramTypeStr, symbol_category_e::VARIABLE, "method_" + currentClassName + "_" + currentMethodName);
        }
        symbolTable.addMethodParameters(currentClassName, currentMethodName, parameterTypes);
    }

    consumeValue(")", "Esperado ')'");
    methodBlock(method);

    currentMethodName = "";

    return method;
}

std::vector<MethodNode> Parser::multipleMethodsDeclarations()
{
    std::vector<MethodNode> methods;
    while (checkValue("public"))
    {
        methods.push_back(methodDeclaration());
    }

    return methods;
}

MainClassNode Parser::mainClassDeclaration()
{
    MainClassNode main;
    consumeValue("class", "Esperado 'class'");
    main.name = consume(token_types_e::IDENTIFIER, "Esperado nome da classe principal").value;

    // Add na tabela de símbolos
    currentClassName = main.name;
    currentMethodName = "main";
    symbolTable.addSymbol(main.name, "class", symbol_category_e::CLASS, "global");
    symbolTable.addSymbol("main", "void", symbol_category_e::METHOD, "class_" + currentClassName);

    consumeValue("{", "Esperado '{' depois do nome da classe");

    consumeValue("public", "Esperado 'public'");
    consumeValue("static", "Esperado 'static'");
    consumeValue("void", "Esperado 'void'");
    consumeValue("main", "Esperado 'main'");

    consumeValue("(", "Esperado '(' depois de main");
    consumeValue("String", "Esperado 'String'");
    consumeValue("[", "Esperado '[' depois de String");
    consumeValue("]", "Esperado ']' depois de '['");
    main.argsName = consume(token_types_e::IDENTIFIER, "Esperado nome do argumento").value;
    
    // Add na tabela de símbolos
    symbolTable.addSymbol(main.argsName, "String[]", symbol_category_e::VARIABLE, "method_" + currentClassName + "_" + currentMethodName);
    
    consumeValue(")", "Esperado ')' depois do parâmetro de main");
    consumeValue("{", "Esperado '{' antes do corpo de main");

    main.commands = commandList();

    consumeValue("}", "Esperado '}' depois do corpo de main");
    consumeValue("}", "Esperado '}' depois da classe principal");

    currentClassName = "";
    currentMethodName = "";

    return main;
}

ClassNode Parser::classDeclaration()
{
    ClassNode classNode;
    consumeValue("class", "Esperado 'class'");
    classNode.name = consume(token_types_e::IDENTIFIER, "Esperado nome da classe").value;

    // Add na tabela de símbolos
    currentClassName = classNode.name;
    symbolTable.addSymbol(classNode.name, "class", symbol_category_e::CLASS, "global");

    if (matchValue("extends"))
    {
        classNode.parentName = consume(token_types_e::IDENTIFIER, "Esperado nome da classe").value;
        // Add na tabela de símbolos
        symbolTable.setParentClass(classNode.name, classNode.parentName);
    }

    consumeValue("{", "Esperado '{' antes do corpo da classe");

    // Add atributos da classa na tabela de simbolos
    classNode.variables = multipleVarsDeclarations();
    for (const auto& var : classNode.variables) {
        std::string varTypeStr = var.type.name + (var.type.isArray ? "[]" : "");
        symbolTable.addSymbol(var.name, varTypeStr, symbol_category_e::VARIABLE, "class_" + currentClassName);
    }

    classNode.methods = multipleMethodsDeclarations();

    consumeValue("}", "Esperado '}' depois do corpo da classe");

    currentClassName = "";

    return classNode;
}

std::vector<ClassNode> Parser::multipleClassDeclarations()
{
    std::vector<ClassNode> classVector;
    while (checkValue("class"))
    {
        classVector.push_back(classDeclaration());
    }

    return classVector;
}

bool Parser::isTypeStart()
{
    return (checkValue("int") || checkValue("boolean") || (check(token_types_e::IDENTIFIER) && checkNext(token_types_e::IDENTIFIER)));
}

TypeNode Parser::type()
{
    TypeNode type;
    if (matchValue("int"))
    {
        if (matchValue("["))
        {
            consumeValue("]", "Esperado ']'");
            type.isArray = true;
        }
        type.name = "int";
    }

    else if (matchValue("boolean"))
    {
        type.name = "boolean";
    }

    else if (check(token_types_e::IDENTIFIER))
    {
        type.name = consume(token_types_e::IDENTIFIER, "Esperado tipo").value;
    }

    else
    {
        type.name = "";
    }

    return type;
}

VarDeclNode Parser::varDeclaration()
{
    VarDeclNode var;
    var.type = type();
    if (var.type.name == "")
    {
        throw std::runtime_error("Esperado tipo");
    }

    var.name = consume(token_types_e::IDENTIFIER, "Esperado nome da variável").value;
    consumeValue(";", "Esperado ';' depois da declaração de variável");
    return var;
}

std::vector<VarDeclNode> Parser::multipleVarsDeclarations()
{
    std::vector<VarDeclNode> vars;
    while (isTypeStart())
    {
        vars.push_back(varDeclaration());
    }

    return vars;
}

std::unique_ptr<ProgramNode> Parser::parse()
{
    auto program = std::make_unique<ProgramNode>();
    program->mainClass = mainClassDeclaration();
    program->classes = multipleClassDeclarations();
    consume(token_types_e::EOF_TOKEN, "Esperado fim do arquivo");
    return program;
}

std::unique_ptr<ExpressionNode> Parser::primaryExpression()
{
    if (check(token_types_e::INTEGER))
    {
        auto node = std::make_unique<IntegerExpressionNode>();
        node->value = std::stoi(advance().value);
        return node;
    }
    else if (matchValue("true"))
    {
        auto node = std::make_unique<BooleanExpressionNode>();
        node->value = true;
        return node;
    }
    else if (matchValue("false"))
    {
        auto node = std::make_unique<BooleanExpressionNode>();
        node->value = false;
        return node;
    }
    else if (matchValue("this"))
    {
        return std::make_unique<ThisExpressionNode>();
    }
    else if (check(token_types_e::IDENTIFIER))
    {
        auto node = std::make_unique<IdentifierExpressionNode>();
        node->name = advance().value;
        return node;
    }
    else if (matchValue("new"))
    {
        if (matchValue("int"))
        {
            auto node = std::make_unique<NewArrayExpressionNode>();
            consumeValue("[", "Esperado '['");
            node->size = expression();
            consumeValue("]", "Esperado ']'");
            return node;
        }
        else if (check(token_types_e::IDENTIFIER))
        {
            auto node = std::make_unique<NewObjectExpressionNode>();
            node->class_name = advance().value;
            consumeValue("(", "Esperado '('");
            consumeValue(")", "Esperado ')'");
            return node;
        }
    }
    else if (matchValue("("))
    {
        auto node = expression();
        consumeValue(")", "Esperado ')'");
        return node;
    }

    throw std::runtime_error("Esperada expressão");
}

std::unique_ptr<ExpressionNode> Parser::postfixExpression()
{
    auto node = primaryExpression();

    while (true)
    {
        if (matchValue("["))
        {
            auto accessNode = std::make_unique<ArrayAccessExpressionNode>();
            accessNode->array = std::move(node);
            accessNode->index = expression();
            consumeValue("]", "Esperado ']'");
            node = std::move(accessNode);
        }
        else if (matchValue("."))
        {
            if (matchValue("length"))
            {
                auto lengthNode = std::make_unique<ArrayLengthExpressionNode>();
                lengthNode->array = std::move(node);
                node = std::move(lengthNode);
            }
            else
            {
                auto callNode = std::make_unique<MethodCallExpressionNode>();
                callNode->object = std::move(node);
                callNode->method_name = consume(token_types_e::IDENTIFIER, "Esperado nome do método depois de '.'").value;
                consumeValue("(", "Esperado '(' depois do nome do método");

                callNode->arguments = listExpression();

                consumeValue(")", "Esperado ')' depois da chamada de método");
                node = std::move(callNode);
            }
        }
        else
        {
            break;
        }
    }

    return node;
}

std::unique_ptr<ExpressionNode> Parser::unaryExpression()
{
    if (matchValue("!"))
    {
        auto node = std::make_unique<UnaryExpressionNode>();
        node->op = "!";
        node->value = unaryExpression();
        return node;
    }
    return postfixExpression();
}

std::unique_ptr<ExpressionNode> Parser::multiplicationExpression()
{
    auto node = unaryExpression();

    while (matchValue("*"))
    {
        auto binaryNode = std::make_unique<BinaryExpressionNode>();
        binaryNode->value1 = std::move(node);
        binaryNode->op = "*";
        binaryNode->value2 = unaryExpression();
        node = std::move(binaryNode);
    }

    return node;
}

std::unique_ptr<ExpressionNode> Parser::additionDifferenceExpression()
{
    auto node = multiplicationExpression();

    while (checkValue("+") || checkValue("-"))
    {
        auto binaryNode = std::make_unique<BinaryExpressionNode>();
        binaryNode->value1 = std::move(node);
        binaryNode->op = advance().value;
        binaryNode->value2 = multiplicationExpression();
        node = std::move(binaryNode);
    }

    return node;
}

std::unique_ptr<ExpressionNode> Parser::comparisonExpression()
{
    auto node = additionDifferenceExpression();

    if (checkValue("<")) // Removi a comparação que add para passar no teste do prof: checkValue(">") || 
    {
        auto binaryNode = std::make_unique<BinaryExpressionNode>();
        binaryNode->value1 = std::move(node);
        binaryNode->op = advance().value;
        binaryNode->value2 = additionDifferenceExpression();
        node = std::move(binaryNode);
    }

    return node;
}

std::unique_ptr<ExpressionNode> Parser::andExpression()
{
    auto node = comparisonExpression();

    while (matchValue("&&"))
    {
        // o ponteiro que estava em node agora pertence a binaryNode->value1
        auto binaryNode = std::make_unique<BinaryExpressionNode>();
        binaryNode->value1 = std::move(node);
        binaryNode->op = "&&";
        binaryNode->value2 = comparisonExpression();
        node = std::move(binaryNode);
    }

    return node;
}

std::unique_ptr<ExpressionNode> Parser::expression()
{
    return andExpression();
}

std::vector<std::unique_ptr<ExpressionNode>> Parser::listExpression()
{
    std::vector<std::unique_ptr<ExpressionNode>> expressions;
    if (checkValue(")"))
    {
        return expressions;
    }
    expressions.push_back(expression());

    while (matchValue(","))
    {
        expressions.push_back(expression());
    }

    return expressions;
}
