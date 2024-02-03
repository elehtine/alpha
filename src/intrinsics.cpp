#include "intrinsics.h"


namespace  intrinsics {

  void plus(std::vector<std::string> args, std::string result,
      AssemblyGenerator* asm_generator) {
    asm_generator->emit("movq " + args[0] + ", %rax");
    asm_generator->emit("addq " + args[1] + ", %rax");
    asm_generator->emit("movq %rax, " + result);
  }

  void minus(std::vector<std::string> args, std::string result,
      AssemblyGenerator* asm_generator) {
    asm_generator->emit("movq " + args[0] + ", %rax");
    asm_generator->emit("subq " + args[1] + ", %rax");
    asm_generator->emit("movq %rax, " + result);
  }

  void multiply(std::vector<std::string> args, std::string result,
      AssemblyGenerator* asm_generator) {
    asm_generator->emit("movq " + args[0] + ", %rax");
    asm_generator->emit("imulq " + args[1] + ", %rax");
    asm_generator->emit("movq %rax, " + result);
  }

  void divide(std::vector<std::string> args, std::string result,
      AssemblyGenerator* asm_generator) {
    asm_generator->emit("movq " + args[0] + ", %rax");
    asm_generator->emit("cqto");
    asm_generator->emit("idivq " + args[1]);
    asm_generator->emit("movq %rax, " + result);
  }

} /* intrinsics */
