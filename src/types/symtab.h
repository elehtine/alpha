#ifndef SYMTAB_H
#define SYMTAB_H

#include <memory>
#include <map>


template<typename T>
class SymTab {
  public:
    SymTab(std::unique_ptr<SymTab> parent);

    void assign_variable(std::string identifier, T value);
    void declare_variable(std::string identifier, T value);
    T get_variable(std::string identifier);

    std::unique_ptr<SymTab> get_parent();

  private:
    std::unique_ptr<SymTab> parent;
    std::map<std::string, T> symbols;
};

#endif
