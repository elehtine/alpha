#include <vector>
#include <memory>

#include "tools/readwrite.h"
#include "tools/exceptions.h"
#include "types/token.h"
#include "parser.h"
#include "interpreter.h"


bool statement(int level) { return level == token::statement; }
bool binary(int level) { return token::statement < level && level < token::primary; }
bool unary(int level) { return token::primary <= level; }

Parser::Parser(std::vector<token::Token*> tokens, Printer* printer
    ): tokens(tokens) {
  root = parse(token::expression);
  printer->print_tree(root.get());
}

std::unique_ptr<ast::Expression> Parser::parse(int level) {
  if (statement(level)) return parse_statement(level);
  if (binary(level)) return parse_binary(level);
  if (unary(level)) return consume()->parse(this);
  throw ParseException("Invalid parse level: " + std::string(*peek()));
}

ast::Expression* Parser::get_ast() {
  return root.get();
}

std::unique_ptr<ast::Expression> Parser::parse_parenthesis() {
  std::unique_ptr<ast::Expression> result = parse(token::expression);
  token::Token* token = consume();
  if (token->get_content() != ")") {
    throw ParseException(token->message({ token::Type::punctuation }));
  }
  return result;
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

std::unique_ptr<ast::Expression> Parser::parse_statement(int level) {
  return parse(level + 1);
}

std::unique_ptr<ast::Expression> Parser::parse_binary(int level) {
  std::unique_ptr<ast::Expression> left = parse(level + 1);
  while (peek()->level() == level) {
    token::Token* token = consume();
    std::unique_ptr<ast::Expression> right = parse(level + 1);
    left = std::make_unique<ast::BinaryOp>(std::move(left),
        token->parse_str(), std::move(right));
  }
  return left;
}
