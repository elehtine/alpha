#include <string>

#include "compiler.h"

#include "tools/readwrite.h"


Compiler::Compiler(std::string source):
  source(source) {
    t = tokeniser.tokenise(source);
    ast = parser.parse(t);
    value = interpreter::interpret(ast.get());
  }

void Compiler::compile(Printer& printer) {
  printer.print_source(source);
  printer.print_tokens(t);
  printer.print_tree(ast.get());
  printer.print_value(value.get());
}
