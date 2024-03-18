#ifndef INTRINSICS_H
#define INTRINSICS_H

#include "asm_generator.h"


namespace intrinsics {

  void unary_minus(std::vector<std::string> args, std::string result,
      AssemblyGenerator* gen);

  void unary_not(std::vector<std::string> args, std::string result,
      AssemblyGenerator* gen);


  void plus(std::vector<std::string> args, std::string result,
      AssemblyGenerator* gen);

  void minus(std::vector<std::string> args, std::string result,
      AssemblyGenerator* gen);

  void multiply(std::vector<std::string> args, std::string result,
      AssemblyGenerator* gen);

  void divide(std::vector<std::string> args, std::string result,
      AssemblyGenerator* gen);

} /* intrinsics */

#endif
