#include "asm_generator.h"


Locals::Locals(std::vector<Instruction*> instructions) {
  for (const Instruction* instruction: instructions) {
    instruction->add_variables(this);
  }
}

void Locals::add_location(const IrVar& variable) {
  if (get_location(variable) != "") return;
  stack_size++;
  std::string location = std::string("-") + std::to_string(stack_size * 8) + std::string("(%rbp)");
  variable_locations[variable] = location;
}

std::string Locals::get_location(const IrVar& variable) {
  return variable_locations[variable];
}

int Locals::get_stack_size() {
  return stack_size;
}

AssemblyGenerator::AssemblyGenerator(std::vector<Instruction*> instructions) {
}

std::vector<std::string> AssemblyGenerator::get_lines() {
  return lines;
}
