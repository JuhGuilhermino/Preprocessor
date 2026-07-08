#include "ThreeAddressCode.h"
#include <iostream>
#include <utility>
#include <stdexcept>

namespace {
std::string symbolName(Symbol* symbol){
    if(symbol == nullptr){
        return "_";
    }

    return symbol->name;
}

std::string opcodeToString(opcode op){
    switch(op){
        case ADD:
            return "+";
        case SUB:
            return "-";
        case MUL:
            return "*";
        case AND:
            return "&&";
        case NOT:
            return "!";
        case ASSIGN:
            return "=";
        case PRINT:
            return "print";
        case RETURN:
            return "return";
        case IF_FALSE:
            return "ifFalse";
        case LABEL:
            return "label";
        case GOTO:
            return "goto";
    }

    return "unknown";
}
}

bool TACList::empty(){
    if(head == nullptr){
        return true;
    }

    return false;
}

void TACList::appendInstruction(std::unique_ptr<Instruction>  inst){
     if(inst == nullptr){
       throw std::logic_error("A instrução não pode ser vazia");
    }
    else if(this->empty()){
        head = std::move(inst);
        tail = head.get();
    }
    else {
        tail->next = std::move(inst);
        tail = tail->next.get();
    }

}

void TACList::concatenate(TACList& otherList){
    if(otherList.empty()){
        return;
    }
    else if (this->empty()){
        head = std::move(otherList.head);
        tail = otherList.tail;
        otherList.tail = nullptr;
    }
    else {
        auto tail_temp = otherList.tail;
        tail->next = std::move(otherList.head);
        tail = tail_temp;   
        otherList.tail = nullptr;
    }
}

void Instruction::print(){
    switch(op){
        case ADD:
        case SUB:
        case MUL:
        case AND:
            std::cout << symbolName(result) << " = "
                      << symbolName(arg1) << " " << opcodeToString(op) << " "
                      << symbolName(arg2) << "\n";
            break;
        case NOT:
            std::cout << symbolName(result) << " = "
                      << opcodeToString(op) << symbolName(arg1) << "\n";
            break;
        case ASSIGN:
            std::cout << symbolName(result) << " = " << symbolName(arg1) << "\n";
            break;
        case PRINT:
            std::cout << "print " << symbolName(arg1) << "\n";
            break;
        case RETURN:
            std::cout << "return " << symbolName(arg1) << "\n";
            break;
        case IF_FALSE:
            std::cout << "ifFalse " << symbolName(arg1)
                      << " goto " << symbolName(result) << "\n";
            break;
        case LABEL:
            std::cout << symbolName(result) << ":\n";
            break;
        case GOTO:
            std::cout << "goto " << symbolName(result) << "\n";
            break;
    }
}

void TACList::print(){
    Instruction* current = head.get();

    while(current != nullptr){
        current->print();
        current = current->next.get();
    }
}
