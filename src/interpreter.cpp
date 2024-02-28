#include "interpreter.h"

#include "types/ast.h"
#include "types/interpretation.h"
#include "tools/readwrite.h"


Interpreter::Interpreter(Expression* tree, Printer* printer):
  tree(tree), printer(printer)
{
  tree->interpret(this);
  printer->print_interpretation(get_interpretations());
}

void Interpreter::add_interpretation(std::unique_ptr<Interpretation> value) {
  interpretations.push_back(std::move(value));
}

std::vector<Interpretation*> Interpreter::get_interpretations() {
  std::vector<Interpretation*> result;
  for (const std::unique_ptr<Interpretation>& interpretation: interpretations) {
    result.push_back(interpretation.get());
  }
  return result;
}
