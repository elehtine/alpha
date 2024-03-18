#ifndef GENERATE_IR_H
#define GENERATE_IR_H

#include <vector>
#include <memory>

#include "types/ast.h"
#include "types/ir.h"
#include "types/symtab.h"


class Printer;


class IrGenerator {
  public:
    IrGenerator(Expression* root, Printer* printer);
    std::vector<Instruction*> get_instructions() const;
    void add_instruction(std::unique_ptr<Instruction> instruction);
    IrVar create_var(Location location);
    std::unique_ptr<Instruction> create_label();

    void declare_variable(std::string identifier, IrVar value);
    IrVar get_variable(std::string identifier);
    void start_block();
    void end_block();

  private:
    std::unique_ptr<SymTab<IrVar>> symtab;
    std::vector<std::unique_ptr<Instruction>> instructions;
    int number = 1;
    Printer* printer;
};

#endif
