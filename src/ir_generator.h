#ifndef GENERATE_IR_H
#define GENERATE_IR_H

#include <vector>
#include <memory>

#include "types/ast.h"
#include "types/ir.h"


class IrGenerator {
  public:
    IrGenerator(ast::Expression* root);
    std::vector<Instruction*> get_instructions();

  private:
    IrVar visit(ast::Expression* node);
    IrVar create_var();
    IrVar add_literal(ast::Literal* literal);
    IrVar add_binary_op(ast::BinaryOp* binary_op);

    std::vector<std::unique_ptr<Instruction>> instructions;
    int number = 1;
};

#endif
