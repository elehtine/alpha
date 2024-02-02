#include <string>

#include "compiler.h"

#include "tools/readwrite.h"

namespace compiler {

  Compiler::Compiler(std::string source):
    source(source), tokeniser(source), parser(tokeniser.get_tokens()) {}

  std::string Compiler::tokens(bool verbal) {
    std::string prefix = verbal ? tokeniser.prefix() : "";
    return prefix + std::string(tokeniser);
  }

  std::string Compiler::tree(bool verbal) {
    std::string prefix = verbal ? parser.prefix() : "";
    return prefix + std::string(parser);
  }

  std::string Compiler::interpret(bool verbal) {
    std::string prefix = verbal ? interpreter.prefix() : "";
    return prefix + std::string(interpreter);
  }

}; /* compiler */

