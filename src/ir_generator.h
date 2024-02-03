#ifndef GENERATE_IR_H
#define GENERATE_IR_H

#include <vector>
#include <memory>

#include "types/ir.h"


class IrGenerator {
  public:
    std::vector<std::unique_ptr<Instruction>> generate();
};

#endif
