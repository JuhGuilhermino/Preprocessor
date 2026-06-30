#include "SymbolTable.h"

std::string SymbolTable::makeKey(const std::string& scope, const std::string& name) {
    return scope + "::" + name;
}

bool SymbolTable::addSymbol(const std::string& name, const std::string& type, symbol_category_e category, const std::string& scope) {
    std::string key = makeKey(scope, name);
    
    if (table.find(key) != table.end()) {
        return false; 
    }

    table[key] = Symbol{name, type, category, scope};
    return true;
}

void SymbolTable::setParentClass(const std::string& className, const std::string& parentClassName) {
    inheritanceMap[className] = parentClassName;
}

std::optional<Symbol> SymbolTable::lookupVariable(const std::string& name, const std::string& currentClass, const std::string& currentMethod) {
    if (!currentMethod.empty()) {
        std::string methodScope = "method_" + currentClass + "_" + currentMethod;
        std::string key = makeKey(methodScope, name);
        if (table.find(key) != table.end()) {
            return table[key];
        }
    }

    std::string classScope = "class_" + currentClass;
    std::string classKey = makeKey(classScope, name);
    if (table.find(classKey) != table.end()) {
        return table[classKey];
    }

    std::string activeClass = currentClass;
    while (inheritanceMap.find(activeClass) != inheritanceMap.end()) {
        std::string parentClass = inheritanceMap[activeClass];
        std::string parentScope = "class_" + parentClass;
        std::string parentKey = makeKey(parentScope, name);
        
        if (table.find(parentKey) != table.end()) {
            return table[parentKey];
        }
        activeClass = parentClass; 
    }

    return std::nullopt;
}

void SymbolTable::print() const {
    std::cout << "\n======================================================================\n";
    std::cout << "                         TABELA DE SÍMBOLOS                           \n";
    std::cout << "======================================================================\n";
    
    // Define larguras fixas para cada coluna alinhar perfeitamente
    std::cout << std::left 
              << std::setw(26) << "Escopo"   << " | "
              << std::setw(12) << "Nome"     << " | "
              << std::setw(10) << "Tipo"     << " | "
              << "Categoria\n";
    std::cout << "----------------------------------------------------------------------\n";

    for (const auto& pair : table) {
        const Symbol& sym = pair.second;
        std::string catStr = (sym.category == symbol_category_e::CLASS) ? "CLASS" :
                             (sym.category == symbol_category_e::METHOD) ? "METHOD" : "VARIABLE";
        
        std::cout << std::left 
                  << std::setw(26) << sym.scope << " | "
                  << std::setw(12) << sym.name  << " | "
                  << std::setw(10) << sym.type  << " | "
                  << catStr << "\n";
    }
    std::cout << "======================================================================\n";
}