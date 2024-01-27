#include <string>

#include "compiler.h"

namespace compiler {

  Compiler::Compiler(std::string source):
    source(source), tokeniser(source), parser(tokeniser.get_tokens()) {}

}; /* compiler */

