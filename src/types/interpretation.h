#ifndef VALUE_H
#define VALUE_H

#include <string>
#include <memory>
#include <map>

#include "type.h"

class Value {
  public:
    Value();
    Value(ValueType type, int value);
    operator std::string() const;

    ValueType type;
    int value;
};


class SymTab {
  public:
    SymTab(std::unique_ptr<SymTab> parent);

    void assign_variable(std::string identifier, Value value);
    void declare_variable(std::string identifier, Value value);
    Value get_variable(std::string identifier);

    std::unique_ptr<SymTab> get_parent();

  private:
    std::unique_ptr<SymTab> parent;
    std::map<std::string, Value> symbols;
};

#endif
