#include <string>

#include "compiler.h"

#include "tools/readwrite.h"


Compiler::Compiler(Source& source, Printer* printer):
  source(source),
  tokeniser(source, printer),
  parser(tokeniser.get_tokens(), printer),
  ir_generator(parser.get_ast(), printer),
  asm_generator(ir_generator.get_instructions(), printer),
  interpreter(parser.get_ast(), printer) {
    printer->print_check(checker::check(parser.get_ast()));
  }
