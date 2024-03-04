#include "ir_generator.h"

#include "tools/exceptions.h"
#include "tools/printer.h"


IrGenerator::IrGenerator(Expression* root, Printer* printer) {
  if (root == nullptr) return;
  root->visit(this);
  printer->print_ir(get_instructions());
}

std::vector<Instruction*> IrGenerator::get_instructions() const {
  std::vector<Instruction*> result;
  for (const std::unique_ptr<Instruction>& instruction: instructions) {
    result.push_back(instruction.get());
  }
  return result;
}

void IrGenerator::add_instruction(
    std::unique_ptr<Instruction> instruction) {
  instructions.push_back(std::move(instruction));
}

IrVar IrGenerator::create_var() {
  IrVar variable("x" + std::to_string(number));
  number++;
  return variable;
}

std::unique_ptr<Instruction> IrGenerator::create_label() {
  std::unique_ptr<Instruction> label = std::make_unique<Label>(number);
  number++;
  return label;
}
