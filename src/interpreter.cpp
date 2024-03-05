#include "interpreter.h"

#include "types/ast.h"
#include "types/interpretation.h"
#include "tools/printer.h"


Interpreter::Interpreter(Expression* tree, Printer* printer):
  tree(tree), printer(printer), symtab(std::make_unique<SymTab>(nullptr))
{
  tree->interpret(this);
  printer->print_interpretation(interpretations);
}

void Interpreter::add_interpretation(Value value) {
  interpretations.push_back(value);
}

void Interpreter::assign_variable(std::string identifier, Value value) {
  symtab->assign_variable(identifier, value);
}

void Interpreter::declare_variable(std::string identifier, Value value) {
  symtab->declare_variable(identifier, value);
}

Value Interpreter::get_variable(std::string identifier) {
  return symtab->get_variable(identifier);
}

void Interpreter::start_block() {
  symtab = std::make_unique<SymTab>(std::move(symtab));
}

void Interpreter::end_block() {
  symtab = symtab->get_parent();
}
