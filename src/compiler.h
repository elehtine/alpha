#ifndef COMPILER_H
#define COMPILER_H

#include <string>
#include <memory>

#include "source.h"
#include "tokens.h"
#include "parser.h"
#include "ir_generator.h"
#include "asm_generator.h"

#include "interpreter.h"
#include "checker.h"

#include "tools/readwrite.h"


class Compiler {
  public:
    Compiler(Source& source, Printer* printer);

  private:
    Source source;
    Tokens tokens;
    Parser parser;
    Interpreter interpreter;
    IrGenerator ir_generator;
    AssemblyGenerator asm_generator;
};

#endif
