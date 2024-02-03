#include "ir_generator.h"

IrGenerator::IrGenerator(ast::Expression* root) {
}

std::vector<Instruction*> IrGenerator::get_instructions() {
  std::vector<Instruction*> result;
  for (std::unique_ptr<Instruction>& instruction: instructions) {
    result.push_back(instruction.get());
  }
  return result;
}
