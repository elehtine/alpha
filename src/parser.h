#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <memory>

#include "types/token.h"
#include "types/ast.h"
#include "interpreter.h"


class Parser {
  public:
    Parser(std::vector<token::Token> tokens, bool verbose);
    ast::Expression* get_ast();

  private:
    std::unique_ptr<ast::Expression> parse_parenthesis(token::Token token);
    std::unique_ptr<ast::Expression> parse_factor();
    std::unique_ptr<ast::Expression> parse_term();
    std::unique_ptr<ast::Expression> parse_expression();

    token::Token peek();
    token::Token consume();

    std::unique_ptr<ast::Expression> root;

    const std::vector<token::Token> tokens;
    const bool verbose;
    size_t position = 0;

};

#endif
