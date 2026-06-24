#ifndef COMPILER_HELPER_H
#define COMPILER_HELPER_H

#include <vector>
#include "lexer.h"

struct Flags
{
    bool print_tokens = false;
    bool stop_first_error = false;
    bool print_sintatic_tree = false;
    bool give_sugestions = false;
    bool print_symbols_table = false;
};

class CompileHelper{
    public:
        void usage();
        bool read_arguments(int argc, char* argv[], Flags& f, std::string& filePath);
        void print_tokens(std::vector<token> tokens);

};

#endif