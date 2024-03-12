#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string>
#include <memory>
#include <vector>

#include "types/ast.h"
#include "types/value.h"
#include "types/symtab.h"


class Interpreter {
  public:
    Interpreter(Expression* tree, Printer* printer);
    void add_interpretation(Value value);

    void assign_variable(std::string identifier, Value value);
    void declare_variable(std::string identifier, Value value);
    Value get_variable(std::string identifier);
    void start_block();
    void end_block();

  private:
    Expression* tree;
    Printer* printer;

    std::unique_ptr<SymTab<Value>> symtab;
    std::vector<Value> interpretations;
};

#endif
