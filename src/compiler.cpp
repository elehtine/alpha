#include <string>

#include "compiler.h"

#include "tools/readwrite.h"


Compiler::Compiler(std::string source, bool verbose): source(source),
  tokeniser(source, verbose), parser(tokeniser.get_tokens(), verbose) {
    tokens = tokeniser.get_tokens();
    ast = parser.parse();
    interpretation = interpreter::interpret(ast.get());
    check = checker::check(ast.get());
  }

void Compiler::compile(Printer& printer) {
  printer.print_source(source);
  printer.print_tokens(tokens);
  printer.print_tree(ast.get());
  printer.print_interpretation(interpretation.get());
  printer.print_check(check);
}
