#include <iostream>
#include "Preprocessor.h"
#include "lexer.h"
#include "parser.h"
//#include "FileHandler.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: ./preprocessador entrada.java saida.java\n";
        return 1;
    }

    // 1. Leitura do arquivo
    std::string filePath = argv[1];
    std::cout << "Arquivo: " << filePath << std::endl;

    // 2. Preprocessamento
    Preprocessor preprocessor;
    std::string code = preprocessor.process(filePath);
    std::cout << "Código processado:\n";
    //std::cout << code << std::endl;

    // 3. Analisador Léxico
    Lexer lexer;
    lexer.setSource(code);
    lexer.mainLoop();
    std::vector<token> tokens = lexer.getTokenList();
    /*std::cout << "\nTokens:\n";
    for (const auto& t : tokens) {
        std::cout << "Tipo: " << (int)t.type
                  << " | Valor: " << t.value
                  << " | Linha: " << t.line << "\n";
    }*/

    // 4. Analisador Sintático
    Parser parser(tokens);
    try {
    parser.parse();
    std::cout << "Código sintaticamente correto!\n";
    } catch (const std::exception& e) {
        std::cerr << "Erro sintático: " << e.what() << "\n";
    }

    return 0;
}