#include <iostream>
#include <string>
#include "Compiler.h"

int main(int argc, char* argv[]) {
    Compiler compiler;
    return compiler.compile(argc, argv);

}
