#ifndef ASM_GENERATOR_H
#define ASM_GENERATOR_H

#include <map>
#include <vector>

#include "types/ir.h"


class Locals {
  public:
    Locals(std::vector<Instruction*> instructions);
    void add_location(const IrVar& variable);
    std::string get_location(const IrVar& variable);
    int get_stack_size();

  private:
    std::map<IrVar, std::string> variable_locations;
    int stack_size = 0;
};

class AssemblyGenerator {
  public:
    AssemblyGenerator(std::vector<Instruction*> instructions);
    std::vector<std::string> get_lines() const;
    void emit(std::string line);
    std::string get_location(const IrVar& variable);

  private:
    Locals locals;
    std::vector<std::string> lines;
};

#endif
