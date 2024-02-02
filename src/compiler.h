#ifndef COMPILER_H
#define COMPILER_H

#include <string>
#include <memory>

#include "tokeniser.h"
#include "parser.h"
#include "interpreter.h"

namespace compiler {

  class Compiler {
    public:
      Compiler(std::string source);

      std::string tokens(bool verbal);
      std::string tree(bool verbal);
      std::string interpret(bool verbal);

    private:
      std::string source;
      Tokeniser tokeniser;
      Parser parser;
      Interpreter interpreter;

      std::vector<token::Token> t;
      std::unique_ptr<ast::Expression> ast;
      std::unique_ptr<value::Value> value;
  };

} /* compiler */

#endif
