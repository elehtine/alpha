#include "interpreter.h"

#include "tools/exceptions.h"
#include "types/interpretation.h"


Interpreter::Interpreter(ast::Expression* tree) {
  if (tree == nullptr) return;
  interpretation = tree->interpret();
}

interpretation::Interpretation* Interpreter::get_interpretation() {
  return interpretation.get();
}
