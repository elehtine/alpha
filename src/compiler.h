#ifndef COMPILER_H
#define COMPILER_H

#include <string>
#include <memory>

#include "types/source.h"
#include "types/token.h"
#include "parser.h"
#include "ir_generator.h"
#include "asm_generator.h"

#include "interpreter.h"
#include "checker.h"


class Printer;


class Compiler {
  public:
    Compiler(Source& source, Printer* printer);

  private:
    Source source;
    std::unique_ptr<Tokens> tokens;
    std::unique_ptr<Expression> tree;
    Checker checker;
    Interpreter interpreter;
    IrGenerator ir_generator;
    AssemblyGenerator asm_generator;
};

#endif
