#ifndef CHECKER_H
#define CHECKER_H

#include "types/ast.h"
#include "types/symtab.h"


class Checker {
  public:
    Checker(Expression* tree, Printer* printer);

    void assign_variable(std::string identifier, ValueType value);
    void declare_variable(std::string identifier, ValueType value);
    ValueType get_variable(std::string identifier);
    void start_block();
    void end_block();

  private:
    Expression* tree;
    Printer* printer;

    std::unique_ptr<SymTab<ValueType>> symtab;
};

#endif
