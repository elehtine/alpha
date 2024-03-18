#include "ir_generator.h"

#include "tools/exceptions.h"
#include "tools/printer.h"


IrGenerator::IrGenerator(Expression* root, Printer* printer): symtab(nullptr), printer(printer) {
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

IrVar IrGenerator::create_var(Location location) {
  IrVar variable(location, "x" + std::to_string(number));
  number++;
  return variable;
}

std::unique_ptr<Instruction> IrGenerator::create_label() {
  std::unique_ptr<Instruction> label = std::make_unique<Label>(number);
  number++;
  return label;
}

void IrGenerator::declare_variable(std::string identifier, IrVar var) {
  symtab->declare_variable(identifier, var);
}

IrVar IrGenerator::get_variable(std::string identifier) {
  return symtab->get_variable(identifier);
}

void IrGenerator::start_block() {
  symtab = std::make_unique<SymTab<IrVar>>(std::move(symtab));
}

void IrGenerator::end_block() {
  symtab = symtab->get_parent();
}
