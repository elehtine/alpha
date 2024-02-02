#include <string>
#include <iostream>

#include "compiler.h"

#include "tools/readwrite.h"

namespace compiler {

  Compiler::Compiler(std::string source):
    source(source) {
      t = tokeniser.tokenise(source);
      ast = parser.parse(t);
      value = interpreter.interpret(ast.get());
    }

  std::string Compiler::tokens(bool verbal) {
    std::string prefix = verbal ? tokeniser.prefix() : "";
    return prefix + std::string(tokeniser);
  }

  std::string Compiler::tree(bool verbal) {
    std::string prefix = verbal ? parser.prefix() : "";
    return prefix + std::string(*ast);
  }

  std::string Compiler::interpret(bool verbal) {
    std::string prefix = verbal ? "Interpretation:\n" : "";
    return prefix + std::string(*value);
  }

}; /* compiler */

