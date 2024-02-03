#include <iostream>

#include "ir_generator.h"

#include "tools/exceptions.h"


IrGenerator::IrGenerator(ast::Expression* root) {
  try {
    visit(root);
  } catch (const IrGenerateException& exception) {}
}

std::vector<Instruction*> IrGenerator::get_instructions() {
  std::vector<Instruction*> result;
  for (std::unique_ptr<Instruction>& instruction: instructions) {
    result.push_back(instruction.get());
  }
  return result;
}

IrVar IrGenerator::visit(ast::Expression* node) {
  ast::Literal* literal;
  ast::BinaryOp* binary_op;
  try {
    literal = dynamic_cast<ast::Literal*>(node);
  } catch (const std::bad_cast& e) {}
  try {
    binary_op = dynamic_cast<ast::BinaryOp*>(node);
  } catch (const std::bad_cast& e) {}
  if (literal != nullptr) return add_literal(literal);
  if (binary_op != nullptr) return add_binary_op(binary_op);
  throw IrGenerateException("Unknown expression");
}

IrVar IrGenerator::create_var() {
  IrVar variable("x" + std::to_string(number));
  number++;
  return variable;
}

IrVar IrGenerator::add_literal(ast::Literal* literal) {
  IrVar variable = create_var();
  instructions.push_back(std::make_unique<LoadIntConst>(literal->get_value(), variable));
  return variable;
}

IrVar IrGenerator::add_binary_op(ast::BinaryOp* binary_op) {
  std::vector<IrVar> arguments = {
    visit(binary_op->get_left()), visit(binary_op->get_right())
  };
  IrVar function = IrVar(binary_op->get_op());
  IrVar result = create_var();
  instructions.push_back(std::make_unique<Call>(function, arguments, result));
  return result;
}
