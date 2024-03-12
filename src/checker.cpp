#include "checker.h"

#include "tools/exceptions.h"
#include "tools/printer.h"


Checker::Checker(Expression* tree, Printer* printer):
  tree(tree), printer(printer), symtab(std::make_unique<SymTab<ValueType>>(nullptr))
{
  tree->check(this);
  printer->print_check(tree);
}

void Checker::assign_variable(std::string identifier, ValueType value) {
  symtab->assign_variable(identifier, value);
}

void Checker::declare_variable(std::string identifier, ValueType value) {
  symtab->declare_variable(identifier, value);
}

ValueType Checker::get_variable(std::string identifier) {
  return symtab->get_variable(identifier);
}

void Checker::start_block() {
  symtab = std::make_unique<SymTab<ValueType>>(std::move(symtab));
}

void Checker::end_block() {
  symtab = symtab->get_parent();
}
