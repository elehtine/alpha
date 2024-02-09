#ifndef GENERATE_IR_H
#define GENERATE_IR_H

#include <vector>
#include <memory>

#include "types/ast.h"
#include "types/ir.h"

#include "tools/readwrite.h"


class IrGenerator {
  public:
    IrGenerator(ast::Expression* root, Printer& printer);
    std::vector<Instruction*> get_instructions() const;
    void add_instruction(std::unique_ptr<Instruction> instruction);
    IrVar create_var();

  private:
    std::vector<std::unique_ptr<Instruction>> instructions;
    int number = 1;
};

#endif
