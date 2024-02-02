#include "interpreter.h"

#include "tools/exceptions.h"


std::unique_ptr<value::Value> Interpreter::interpret(ast::Expression* tree) {
  return std::move(tree->interpret());
}
