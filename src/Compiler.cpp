#include "Compiler.h"
#include "ASTPrinter.h"
#include <iostream>

int Compiler::compile(int argc, char* argv[]){
    std::string filePath;
    if (!helper.read_arguments(argc, argv, flags, filePath)) {
        return 1;
    }
    std::string code;

    try{
    std::vector<std::string> lines;
    try {
        lines = file_handler.readFile(filePath);
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
        
        for (const auto& line : lines) {
            code += line + "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Erro na leitura do arquivo: " << e.what() << "\n";
        return 1;
    }


    std::vector<token> tokens;

    try{
        lexer.setSource(code);

        if(flags.stop_first_error){
            lexer.mainLoop(true);
        }
        else{
            lexer.mainLoop();
        }
        
        tokens = lexer.getTokenList();

        if(flags.print_tokens){
            helper.print_tokens(tokens);
        }

    } catch (const std::exception& e) {
        std::cerr << "Erro léxico: " << e.what() << "\n";
        return 1;
    }

    try{
        Parser parser(tokens);
        ast = parser.parse();
        if(flags.print_sintatic_tree){
            ASTPrinter printer(std::cout);
            printer.print(*ast);
        }
        if (flags.print_symbols_table) {
            parser.getSymbolTable().print();
        }
        std::cout << "Codigo sintaticamente correto!\n";
    } catch (const std::exception& e) {
        std::cerr << "Erro sintático: " << e.what() << "\n";
        return 1;
    }

    return 0;

    
}
