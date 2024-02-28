#include "interpreter.h"

#include "types/ast.h"
#include "types/interpretation.h"
#include "tools/readwrite.h"


Interpreter::Interpreter(Expression* tree, Printer* printer):
  tree(tree), printer(printer)
{}

std::unique_ptr<interpretation::Interpretation> Interpreter::interpret() {
  return std::make_unique<interpretation::Integer>(42);
}
