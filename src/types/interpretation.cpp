#include <string>

#include "interpretation.h"
#include "../tools/exceptions.h"


Integer::Integer(int value): value(value) {}
Integer::operator int() const { return value; }
Integer::operator bool() const {
  throw InterpretException("Expected bool, got int");
}
Integer::operator std::string() const {
  return std::to_string(value);
}
std::unique_ptr<Interpretation> Integer::clone() const {
  return std::make_unique<Integer>(value);
}

Boolean::Boolean(bool value): value(value) {}
Boolean::operator bool() const { return value; }
Boolean::operator int() const {
  throw InterpretException("Expected int, got bool");
}
Boolean::operator std::string() const {
  return "TRUE";
}
std::unique_ptr<Interpretation> Boolean::clone() const {
  return std::make_unique<Boolean>(value);
}


SymTab::SymTab(std::unique_ptr<SymTab> parent):
  parent(std::move(parent))
{}

void SymTab::assign_variable(std::string identifier, std::unique_ptr<Interpretation> value) {
  symbols[identifier] = std::move(value);
}

std::unique_ptr<Interpretation> SymTab::get_variable(std::string identifier) {
  std::unique_ptr<Interpretation>& value = symbols[identifier];
  if (value) return value->clone();
  if (parent) return parent->get_variable(identifier);
  return std::make_unique<Integer>(0);
}

std::unique_ptr<SymTab> SymTab::get_parent() {
  return std::move(parent);
}
