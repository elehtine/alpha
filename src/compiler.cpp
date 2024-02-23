#include <string>

#include "compiler.h"
#include "tokeniser.h"

#include "tools/readwrite.h"


Compiler::Compiler(Source& source, Printer* printer):
  source(source),
  tokens(Tokeniser(source, printer).tokenise()),
  parser(*tokens.get(), printer),
  interpreter(parser.get_ast(), printer),
  ir_generator(parser.get_ast(), printer),
  asm_generator(ir_generator.get_instructions(), printer) {
    printer->print_check(checker::check(parser.get_ast()));
  }
