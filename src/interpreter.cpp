#include "interpreter.h"

#include "types/ast.h"
#include "types/interpretation.h"
#include "tools/printer.h"


Interpreter::Interpreter(Expression* tree, Printer* printer):
  tree(tree), printer(printer), symtab(std::make_unique<SymTab>(nullptr))
{
  tree->interpret(this);
  printer->print_interpretation(get_interpretations());
}

void Interpreter::add_interpretation(std::unique_ptr<Interpretation> value) {
  interpretations.push_back(std::move(value));
}

void Interpreter::assign_variable(std::string identifier, std::unique_ptr<Interpretation> value) {
  symtab->assign_variable(identifier, std::move(value));
}

std::unique_ptr<Interpretation> Interpreter::Interpreter::get_variable(std::string identifier) {
  return symtab->get_variable(identifier);
}

void Interpreter::start_block() {
  symtab = std::make_unique<SymTab>(std::move(symtab));
}

void Interpreter::end_block() {
  symtab = symtab->get_parent();
}

std::vector<Interpretation*> Interpreter::get_interpretations() {
  std::vector<Interpretation*> result;
  for (const std::unique_ptr<Interpretation>& interpretation: interpretations) {
    result.push_back(interpretation.get());
  }
  return result;
}
