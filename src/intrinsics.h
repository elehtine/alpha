#ifndef INTRINSICS_H
#define INTRINSICS_H

#include "asm_generator.h"


namespace intrinsics {

  void unary_minus(std::vector<std::string> args, std::string res, AssemblyGenerator* gen);
  void unary_not(std::vector<std::string> args, std::string res, AssemblyGenerator* gen);


  void plus(std::vector<std::string> args, std::string res, AssemblyGenerator* gen);
  void minus(std::vector<std::string> args, std::string res, AssemblyGenerator* gen);
  void multiply(std::vector<std::string> args, std::string res, AssemblyGenerator* gen);
  void divide(std::vector<std::string> args, std::string res, AssemblyGenerator* gen);
  void remainder(std::vector<std::string> args, std::string res, AssemblyGenerator* gen);


  void comparison(std::vector<std::string> args, std::string res, AssemblyGenerator* gen, std::string instruction);
  void equal(std::vector<std::string> args, std::string res, AssemblyGenerator* gen);
  void not_equal(std::vector<std::string> args, std::string res, AssemblyGenerator* gen);
  void less(std::vector<std::string> args, std::string res, AssemblyGenerator* gen);
  void less_or_equal(std::vector<std::string> args, std::string res, AssemblyGenerator* gen);
  void greater(std::vector<std::string> args, std::string res, AssemblyGenerator* gen);
  void greater_or_equal(std::vector<std::string> args, std::string res, AssemblyGenerator* gen);

} /* intrinsics */

#endif
