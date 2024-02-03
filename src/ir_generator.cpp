#include <iostream>

#include "ir_generator.h"

#include "tools/exceptions.h"


IrGenerator::IrGenerator(ast::Expression* root) {
  try {
    root->visit(this);
  } catch (const IrGenerateException& exception) {}
}

std::vector<Instruction*> IrGenerator::get_instructions() const {
  std::vector<Instruction*> result;
  for (const std::unique_ptr<Instruction>& instruction: instructions) {
    result.push_back(instruction.get());
  }
  return result;
}

void IrGenerator::add_instruction(std::unique_ptr<Instruction> instruction) {
  instructions.push_back(std::move(instruction));
}

IrVar IrGenerator::create_var() {
  IrVar variable("x" + std::to_string(number));
  number++;
  return variable;
}
