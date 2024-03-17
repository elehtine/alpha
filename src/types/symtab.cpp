#include "symtab.h"
#include "../tools/exceptions.h"

#include "../interpreter.h"
#include "ir.h"


template<typename T>
SymTab<T>::SymTab(std::unique_ptr<SymTab> parent):
  parent(std::move(parent))
{}

template<typename T>
void SymTab<T>::assign_variable(std::string identifier, T value) {
  if (symbols.count(identifier)) {
    symbols.erase(identifier);
    symbols.insert({ identifier,  value });
    return;
  }
  if (parent) {
    parent->assign_variable(identifier, value);
    return;
  }
  throw InterpretException("Undeclared identifier in assignment: " + identifier);
}

template<typename T>
void SymTab<T>::declare_variable(std::string identifier, T value) {
  if (symbols.count(identifier)) throw SymTabException();
  symbols.insert({ identifier, value });
}

template<typename T>
T SymTab<T>::get_variable(std::string identifier) {
  if (symbols.count(identifier)) return symbols[identifier];
  if (parent) return parent->get_variable(identifier);
  throw InterpretException("Undeclared identifier used: " + identifier);
}

template<typename T>
std::unique_ptr<SymTab<T>> SymTab<T>::get_parent() {
  return std::move(parent);
}

template class SymTab<Value>;
template class SymTab<ValueType>;
template class SymTab<IrVar>;
