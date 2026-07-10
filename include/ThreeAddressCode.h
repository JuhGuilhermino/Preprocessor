#ifndef THREE_ADDRESS_CODE_H
#define THREE_ADDRESS_CODE_H

#include <cstdint>
#include <memory>
#include "SymbolTable.h"

enum opcode : std::uint8_t{
    ADD, 
    SUB, 
    MUL,
    LT,
    AND,
    NOT,
    ASSIGN,
    ARRAY_LOAD,
    ARRAY_STORE,
    ARRAY_LENGTH,
    NEW_ARRAY,
    NEW_OBJECT,
    PARAM,
    CALL,
    PRINT,
    RETURN,
    IF_FALSE,
    LABEL,
    GOTO
};


struct Instruction{
    opcode op;
    Symbol* arg1;
    Symbol* arg2;
    Symbol* result;
    std::unique_ptr<Instruction> next;

    Instruction(opcode op, Symbol* arg1, Symbol* arg2, Symbol* result)
        : op(op), arg1(arg1), arg2(arg2), result(result), next(nullptr) {}
    void print();
};


struct TACList{
    std::unique_ptr<Instruction> head;
    Instruction* tail;

    void appendInstruction(std::unique_ptr<Instruction>  inst);
    void concatenate(TACList& otherList);
    bool empty();
    void print();

    TACList() : head(nullptr), tail(nullptr) {}

};

#endif
