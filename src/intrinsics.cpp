#include "intrinsics.h"


namespace  intrinsics {

  void unary_minus(std::vector<std::string> args, std::string result,
      AssemblyGenerator* gen) {
    gen->emit("movq " + args[0] + ", %rax");
    gen->emit("negq %rax");
    gen->emit("movq %rax, " + result);
  }

  void unary_not(std::vector<std::string> args, std::string result,
      AssemblyGenerator* gen) {
    gen->emit("movq " + args[0] + ", %rax");
    gen->emit("xorq $1, %rax");
    gen->emit("movq %rax, " + result);
  }


  void plus(std::vector<std::string> args, std::string result,
      AssemblyGenerator* gen) {
    gen->emit("movq " + args[0] + ", %rax");
    gen->emit("addq " + args[1] + ", %rax");
    gen->emit("movq %rax, " + result);
  }

  void minus(std::vector<std::string> args, std::string result,
      AssemblyGenerator* gen) {
    gen->emit("movq " + args[0] + ", %rax");
    gen->emit("subq " + args[1] + ", %rax");
    gen->emit("movq %rax, " + result);
  }

  void multiply(std::vector<std::string> args, std::string result,
      AssemblyGenerator* gen) {
    gen->emit("movq " + args[0] + ", %rax");
    gen->emit("imulq " + args[1] + ", %rax");
    gen->emit("movq %rax, " + result);
  }

  void divide(std::vector<std::string> args, std::string result,
      AssemblyGenerator* gen) {
    gen->emit("movq " + args[0] + ", %rax");
    gen->emit("cqto");
    gen->emit("idivq " + args[1]);
    gen->emit("movq %rax, " + result);
  }

} /* intrinsics */
