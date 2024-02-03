#include <string>

#include "compiler.h"

#include "tools/readwrite.h"


Compiler::Compiler(std::string source, bool verbose): source(source),
  tokeniser(source, verbose), parser(tokeniser.get_tokens(), verbose),
  ir_generator(parser.get_ast()), asm_generator(ir_generator.get_instructions()),
  interpreter(parser.get_ast()) {}

void Compiler::compile(Printer& printer) {
  printer.print_source(source);
  printer.print_tokens(tokeniser.get_tokens());
  printer.print_tree(parser.get_ast());
  printer.print_interpretation(interpreter.get_interpretation());
  printer.print_check(checker::check(parser.get_ast()));
  printer.print_ir(ir_generator.get_instructions());
  printer.print_asm(asm_generator.get_lines());
}
