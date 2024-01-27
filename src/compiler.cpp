#include <string>

#include "compiler.h"

#include "tools/readwrite.h"

namespace compiler {

  Compiler::Compiler(std::string source):
    source(source), tokeniser(source), parser(tokeniser.get_tokens()) {}

  std::string Compiler::tokens() {
    return to_string(tokeniser.get_tokens());
  }

  std::string Compiler::tree() {
    return to_string(parser.get_tree());
  }


}; /* compiler */

