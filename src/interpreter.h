#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string>

#include "types/ast.h"
#include "types/value.h"


class Interpreter {
  public:
    std::unique_ptr<value::Value> interpret(ast::Expression* tree);
};

#endif
