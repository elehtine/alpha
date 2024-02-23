#ifndef INTRINSICS_H
#define INTRINSICS_H

#include "asm_generator.h"


namespace intrinsics {

  void plus(std::vector<std::string> args, std::string result,
      AssemblyGenerator* asm_generator);

  void minus(std::vector<std::string> args, std::string result,
      AssemblyGenerator* asm_generator);

  void multiply(std::vector<std::string> args, std::string result,
      AssemblyGenerator* asm_generator);

  void divide(std::vector<std::string> args, std::string result,
      AssemblyGenerator* asm_generator);

} /* intrinsics */

#endif
