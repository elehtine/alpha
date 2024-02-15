#include "interpreter.h"

#include "tools/exceptions.h"
#include "types/interpretation.h"


Interpreter::Interpreter(ast::Expression* tree, Printer* printer) {
  if (tree == nullptr) return;
  interpretation = tree->interpret();
  printer->print_interpretation(interpretation.get());
}

interpretation::Interpretation* Interpreter::get_interpretation() {
  return interpretation.get();
}
