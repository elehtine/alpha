#include "intrinsics.h"


namespace  intrinsics {

  void unary_minus(std::vector<std::string> args, std::string res, AssemblyGenerator* gen) {
    gen->emit("movq " + args[0] + ", %rax");
    gen->emit("negq %rax");
    gen->emit("movq %rax, " + res);
  }

  void unary_not(std::vector<std::string> args, std::string res, AssemblyGenerator* gen) {
    gen->emit("movq " + args[0] + ", %rax");
    gen->emit("xorq $1, %rax");
    gen->emit("movq %rax, " + res);
  }


  void plus(std::vector<std::string> args, std::string res, AssemblyGenerator* gen) {
    gen->emit("movq " + args[0] + ", %rax");
    gen->emit("addq " + args[1] + ", %rax");
    gen->emit("movq %rax, " + res);
  }

  void minus(std::vector<std::string> args, std::string res, AssemblyGenerator* gen) {
    gen->emit("movq " + args[0] + ", %rax");
    gen->emit("subq " + args[1] + ", %rax");
    gen->emit("movq %rax, " + res);
  }

  void multiply(std::vector<std::string> args, std::string res, AssemblyGenerator* gen) {
    gen->emit("movq " + args[0] + ", %rax");
    gen->emit("imulq " + args[1] + ", %rax");
    gen->emit("movq %rax, " + res);
  }

  void divide(std::vector<std::string> args, std::string res, AssemblyGenerator* gen) {
    gen->emit("movq " + args[0] + ", %rax");
    gen->emit("cqto");
    gen->emit("idivq " + args[1]);
    gen->emit("movq %rax, " + res);
  }

  void remainder(std::vector<std::string> args, std::string res, AssemblyGenerator* gen) {
    gen->emit("movq " + args[0] + ", %rax");
    gen->emit("cqto");
    gen->emit("idivq " + args[1]);
    gen->emit("movq %rdx, " + res);
  }


  void comparison(std::vector<std::string> args, std::string res, AssemblyGenerator* gen, std::string instruction) {
    gen->emit("xor %rax, %rax");
    gen->emit("movq " + args[0] + ", %rdx");
    gen->emit("cmpq " + args[1] + ", %rdx");
    gen->emit(instruction + " %al");
    gen->emit("movq %rax, " + res);
  }

  void equal(std::vector<std::string> args, std::string res, AssemblyGenerator* gen) {
    comparison(args, res, gen, "sete");
  }

  void not_equal(std::vector<std::string> args, std::string res, AssemblyGenerator* gen) {
    comparison(args, res, gen, "setne");
  }

  void less(std::vector<std::string> args, std::string res, AssemblyGenerator* gen) {
    comparison(args, res, gen, "setl");
  }

  void less_or_equal(std::vector<std::string> args, std::string res, AssemblyGenerator* gen) {
    comparison(args, res, gen, "setle");
  }

  void greater(std::vector<std::string> args, std::string res, AssemblyGenerator* gen) {
    comparison(args, res, gen, "setg");
  }

  void greater_or_equal(std::vector<std::string> args, std::string res, AssemblyGenerator* gen) {
    comparison(args, res, gen, "setge");
  }



} /* intrinsics */
