#include "CompileHelper.h"
#include <iostream>
#include <string>


void CompileHelper::usage(){
     std::cout
        << "Uso: ./preprocessador [opções] <arquivo>\n\n"
        << "Opções:\n"
        << "  -h, --help  Mostra esta ajuda.\n"
        << "  -lt         Imprime a lista de tokens.\n"
        << "  -pe         Para no primeiro erro léxico.\n"
        << "  -as         Imprime a árvore sintática abstrata.\n"
        << "  -c          Mostra sugestões de correção léxica e sintática.\n"
        << "  -ts         Imprime a tabela de símbolos após a análise sintática.\n";
}


bool CompileHelper::read_arguments(int argc, char* argv[], Flags& f, std::string& filePath){
    if (argc < 2) {
        usage();
        return false;
    }
    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help")
        {
            usage();
            return false;
        }
        else if (arg == "-lt")
        {
            f.print_tokens = true;
        }
        else if (arg == "-pe")
        {
            f.stop_first_error = true;
        }
        else if (arg == "-as")
        {
            f.print_sintatic_tree = true;
        }
        else if (arg == "-c")
        {
            f.give_sugestions = true;
        }
        else if (arg == "-ts")
        {
            f.print_symbols_table = true;
        }
        else if (!arg.empty() && arg[0] == '-')
        {
            std::cerr << "Opção desconhecida: " << arg << "\n\n";
            usage();
            return false;
        }
        else
        {
            filePath = arg;
        }
    }

    if (filePath.empty())
    {
        std::cerr << "Erro: informe o arquivo de entrada.\n\n";
        usage();
        return false;
    }

    return true;
}

void CompileHelper::print_tokens(std::vector<token> tokens){
    for (const auto& t : tokens) {
            std::cout << "Tipo: " << (int)t.type
                      << " | Valor: " << t.value
                      << " | Linha: " << t.line
                      << " | Coluna: " << t.column << "\n";
        }

}
