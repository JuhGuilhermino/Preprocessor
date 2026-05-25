#include <iostream>
#include "Preprocessor.h"
//#include "FileHandler.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: ./preprocessador entrada.java saida.java\n";
        return 1;
    }

    std::string filePath = argv[1];

    std::cout << "Arquivo: " << filePath << std::endl;

    Preprocessor preprocessor;

    std::string result = preprocessor.process(filePath);

    std::cout << "Código processado:\n";
    std::cout << result << std::endl;

    return 0;
}