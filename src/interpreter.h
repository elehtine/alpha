#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string>
#include <memory>
#include <vector>

#include "types/ast.h"
#include "types/interpretation.h"


class Interpreter {
  public:
    Interpreter(Expression* tree, Printer* printer);
    void add_interpretation(std::unique_ptr<Interpretation> value);

    void assign_variable(std::string identifier, std::unique_ptr<Interpretation> value);
    std::unique_ptr<Interpretation> get_variable(std::string identifier);
    void start_block();
    void end_block();

  private:
    std::vector<Interpretation*> get_interpretations();

    Expression* tree;
    Printer* printer;

    std::unique_ptr<SymTab> symtab;
    std::vector<std::unique_ptr<Interpretation>> interpretations;
};

#endif
