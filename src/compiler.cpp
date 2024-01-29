#include <string>

#include "compiler.h"

#include "tools/readwrite.h"

namespace compiler {

  Compiler::Compiler(std::string source):
    source(source), tokeniser(source), parser(tokeniser.get_tokens()) {}

  std::string Compiler::tokens() {
    return std::string(tokeniser);
  }

  std::string Compiler::tree() {
    return std::string(parser);
  }

}; /* compiler */

