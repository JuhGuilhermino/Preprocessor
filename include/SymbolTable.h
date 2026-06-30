#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <optional>
#include <iostream>
#include <iomanip>

enum class symbol_category_e {
    CLASS,
    METHOD,
    VARIABLE
};

struct Symbol {
    std::string name;
    std::string type; // "int", "boolean", "int[]", ou nome da classe
    symbol_category_e category;
    std::string scope; // Ex: "global", "class_A", "method_A_get"
};

class SymbolTable {
    // Guarda todos os símbolos indexados por uma chave composta: "escopo::nome_do_simbolo"
    std::unordered_map<std::string, Symbol> table;

    // Guarda o mapeamento de herança: chave = nome da classe, valor = nome da classe pai (extends)
    std::unordered_map<std::string, std::string> inheritanceMap;

    std::string makeKey(const std::string& scope, const std::string& name);

public:
    bool addSymbol(const std::string& name, const std::string& type, symbol_category_e category, const std::string& scope);

    void setParentClass(const std::string& className, const std::string& parentClassName);

    std::optional<Symbol> lookupVariable(const std::string& name, const std::string& currentClass, const std::string& currentMethod);

    void clear() { table.clear(); inheritanceMap.clear(); }

    void print() const;
};

#endif