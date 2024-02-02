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
    std::vector<std::unique_ptr<ast::Expression>>& get_tree();

    std::string prefix() const;
    operator std::string() const;

  private:
    std::unique_ptr<ast::Literal> parse_literal(token::Token token);
    std::unique_ptr<ast::Identifier> parse_identifier(token::Token token);
    std::unique_ptr<ast::Expression> parse_term();
    std::unique_ptr<ast::Expression> parse_expression();
    void parse_lines();

    token::Token peek();
    token::Token consume();

    std::vector<std::unique_ptr<ast::Expression>> tree;

    std::vector<token::Token> tokens;
    size_t position = 0;
    std::string error = "";
};

#endif
