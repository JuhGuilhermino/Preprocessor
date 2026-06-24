#include "CompileHelper.h"
#include <iostream>
#include <string>


void CompileHelper::usage(){
     std::cout
        << "Uso: ./preprocessador [opcoes] <arquivo>\n\n"
        << "Opcoes:\n"
        << "  -h, --help  Mostra esta ajuda.\n"
        << "  -lt         Imprime a lista de tokens.\n"
        << "  -pe         Para no primeiro erro lexico.\n"
        << "  -as         Imprime a arvore sintatica abstrata.\n"
        << "  -c          Mostra sugestoes de correcao lexica e sintatica.\n"
        << "  -ts         Imprime a tabela de simbolos apos a analise sintatica.\n";
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
            std::cerr << "Opcao desconhecida: " << arg << "\n\n";
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
