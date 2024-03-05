#include <string>

#include "compiler.h"
#include "tokeniser.h"

#include "tools/printer.h"


Compiler::Compiler(Source& source, Printer* printer):
  source(source),
  tokens(Tokeniser(source, printer).tokenise()),
  tree(Parser(*tokens.get(), printer).parse()),
  checker(tree.get(), printer),
  interpreter(tree.get(), printer),
  ir_generator(tree.get(), printer),
  asm_generator(ir_generator.get_instructions(), printer)
{}
