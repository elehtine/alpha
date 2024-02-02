#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <memory>

#include "types/token.h"
#include "types/ast.h"
#include "interpreter.h"


class Parser {
  public:
    std::unique_ptr<ast::Expression> parse(std::vector<token::Token> tokens);

  private:
    std::unique_ptr<ast::Literal> parse_literal(token::Token token);
    std::unique_ptr<ast::Identifier> parse_identifier(token::Token token);
    std::unique_ptr<ast::Expression> parse_term();
    std::unique_ptr<ast::Expression> parse_expression();

    token::Token peek();
    token::Token consume();

    ast::Expression* root;

    std::vector<token::Token> tokens;
    size_t position = 0;
    std::string error = "";
};

#endif
