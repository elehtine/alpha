#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <memory>

#include "types/token.h"
#include "types/ast.h"
#include "interpreter.h"


class Parser {
  public:
    Parser(std::vector<token::Token> tokens);
  std::shared_ptr<ast::Expression> get_tree();

    std::string prefix() const;
    operator std::string() const;

  private:
    std::shared_ptr<ast::Literal> parse_literal(token::Token token);
    std::shared_ptr<ast::Identifier> parse_identifier(token::Token token);
    std::shared_ptr<ast::Expression> parse_term();
    std::shared_ptr<ast::Expression> parse_expression();

    token::Token peek();
    token::Token consume();

    std::shared_ptr<ast::Expression> root;

    std::vector<token::Token> tokens;
    size_t position = 0;
    std::string error = "";
};

#endif
