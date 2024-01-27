#ifndef COMPILER_H
#define COMPILER_H

#include <string>

#include "tokeniser.h"
#include "parser.h"

namespace compiler {

  class Compiler {
    public:
      Compiler(std::string source);

      std::string tokens();
      std::string tree();

    private:
      std::string source;
      tokeniser::Tokeniser tokeniser;
      parser::Parser parser;
  };

} /* compiler */

#endif
