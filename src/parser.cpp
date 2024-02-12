#include <vector>
#include <memory>
#include <iostream>

#include "tools/readwrite.h"
#include "tools/exceptions.h"
#include "types/token.h"
#include "parser.h"
#include "interpreter.h"


Parser::Parser(std::vector<token::Token*> tokens, Printer& printer
    ): tokens(tokens) {
  root = parse_expression();

  token::Token* token = consume();
  if (token->get_type() != token::Type::eof) {
    throw ParseException(token->message({ token::Type::eof }));
  }
  printer.print_tree(root.get());
}

ast::Expression* Parser::get_ast() {
  return root.get();
}

std::unique_ptr<ast::Expression> Parser::parse_parenthesis() {
  std::unique_ptr<ast::Expression> result = parse_expression();
  token::Token* token = consume();
  if (token->get_content() != ")") {
    throw ParseException(token->message({ token::Type::punctuation }));
  }
  return result;
}

std::unique_ptr<ast::Expression> Parser::parse_term() {
  token::Token* token = consume();
  return token->parse(this);
}

std::unique_ptr<ast::Expression> Parser::parse_factor() {
  std::unique_ptr<ast::Expression> left = parse_term();
  while (peek()->get_content() == "*" || peek()->get_content() == "/") {
    token::Token* token = consume();
    std::unique_ptr<ast::Expression> right = parse_term();
    left = std::make_unique<ast::BinaryOp>(std::move(left),
        token->parse_str(), std::move(right));
  }
  return left;
}

std::unique_ptr<ast::Expression> Parser::parse_expression() {
  std::unique_ptr<ast::Expression> left = parse_factor();
  while (peek()->get_content() == "+" || peek()->get_content() == "-") {
    token::Token* token = consume();
    std::unique_ptr<ast::Expression> right = parse_factor();
    left = std::make_unique<ast::BinaryOp>(std::move(left),
        token->parse_str(), std::move(right));
  }
  return left;
}

token::Token* Parser::peek() {
  while (tokens[std::min(position, tokens.size() - 1)]->get_type() == token::Type::whitespace) {
    position++;
  }
  return tokens[std::min(position, tokens.size() - 1)];
}

token::Token* Parser::consume() {
  token::Token* token = peek();
  position++;
  return token;
}
