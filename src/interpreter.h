#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string>
#include <memory>

#include "types/ast.h"
#include "types/interpretation.h"

#include "tools/readwrite.h"


class Interpreter {
  public:
    Interpreter(ast::Expression* tree, Printer& printer);
    interpretation::Interpretation* get_interpretation();
  private:
    std::unique_ptr<interpretation::Interpretation> interpretation;
};

#endif
