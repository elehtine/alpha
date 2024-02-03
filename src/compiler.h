#ifndef COMPILER_H
#define COMPILER_H

#include <string>
#include <memory>

#include "tokeniser.h"
#include "parser.h"
#include "ir_generator.h"
#include "asm_generator.h"

#include "interpreter.h"
#include "checker.h"

#include "tools/readwrite.h"


class Compiler {
  public:
    Compiler(std::string source, bool verbose);
    void compile(Printer& printer);

  private:
    std::string source;
    Tokeniser tokeniser;
    Parser parser;
    IrGenerator ir_generator;
    AssemblyGenerator asm_generator;
    Interpreter interpreter;
};

#endif
