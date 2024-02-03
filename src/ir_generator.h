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
    std::vector<std::unique_ptr<Instruction>> instructions;
};

#endif
