#include "asm_generator.h"
#include "tools/readwrite.h"


Locals::Locals(std::vector<Instruction*> instructions) {
  for (const Instruction* instruction: instructions) {
    instruction->add_variables(this);
  }
}

void Locals::add_location(const IrVar& variable) {
  if (get_location(variable) != "") return;
  stack_size++;
  std::string location = "-" + std::to_string(stack_size * 8) + "(%rbp)";
  variable_locations[variable] = location;
}

std::string Locals::get_location(const IrVar& variable) {
  return variable_locations[variable];
}

int Locals::get_stack_size() {
  return stack_size;
}

AssemblyGenerator::AssemblyGenerator(
    std::vector<Instruction*> instructions, Printer& printer
    ): locals(instructions) {
  std::string start = read("resources/start.s");
  std::string end = read("resources/end.s");

  int last = 0;
  for (size_t index = 0; index < start.size(); index++) {
    if (start[index] == '\n') {
      lines.push_back(start.substr(last, index-last));
      last = index + 1;
    }
  }

  emit("subq $" + std::to_string(8 * locals.get_stack_size()) + ", %rsp");
  if (locals.get_stack_size() % 2 == 1) emit("subq $8, %rsp");
  emit("");

  for (const Instruction* instruction: instructions) {
    instruction->to_asm(this);
    emit("");
  }

  last = 0;
  for (size_t index = 0; index < end.size(); index++) {
    if (end[index] == '\n') {
      lines.push_back(end.substr(last, index-last));
      last = index + 1;
    }
  }

  printer.print_asm(lines);
}

std::vector<std::string> AssemblyGenerator::get_lines() const {
  return lines;
}

void AssemblyGenerator::emit(std::string line) {
  lines.push_back(line);
}

std::string AssemblyGenerator::get_location(const IrVar& variable) {
  return locals.get_location(variable);
}
