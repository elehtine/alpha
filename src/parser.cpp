#include <vector>
#include <memory>

#include "tools/readwrite.h"
#include "tools/exceptions.h"
#include "types/token.h"
#include "parser.h"
#include "interpreter.h"


bool binary(int level) {
  return level < token::primary;
}
bool unary(int level) { return token::primary <= level; }

Parser::Parser(Tokens& tokens, Printer* printer): tokens(tokens) {
  root = parse();
  printer->print_tree(root.get());
}

std::unique_ptr<ast::Expression> Parser::parse() {
  if (tokens.peek()->get_content() == "if") return parse_condition();
  return parse_binary(token::expression);
}

ast::Expression* Parser::get_ast() {
  return root.get();
}

std::unique_ptr<ast::Expression> Parser::parse_parenthesis() {
  std::unique_ptr<ast::Expression> result = parse();
  token::Token* token = tokens.consume();
  if (token->get_content() != ")") {
    throw ParseException(token->message({ token::Type::punctuation }));
  }
  return result;
}

std::unique_ptr<ast::Expression> Parser::parse_condition() {
  token::Token* if_token = tokens.consume();
  std::unique_ptr<ast::Expression> condition = parse();
  token::Token* then_token = tokens.consume();
  std::unique_ptr<ast::Expression> then_expression = parse();
  token::Token* else_token = tokens.consume();
  std::unique_ptr<ast::Expression> else_expression = parse();

  if (if_token->get_content() != "if") {
    throw ParseException("Expected if, got "+if_token->get_content());
  }
  if (then_token->get_content() != "then") {
    throw ParseException("Expected then, got "+then_token->get_content());
  }
  if (else_token->get_content() != "else") {
    throw ParseException("Expected else, got "+else_token->get_content());
  }
  return std::make_unique<ast::IfThenElse>(std::move(condition),
        std::move(then_expression), std::move(else_expression));
}

std::unique_ptr<ast::Expression> Parser::parse_binary(int level) {
  if (level == token::primary) return parse_primary();
  std::unique_ptr<ast::Expression> left = parse_binary(level + 1);
  while (tokens.peek()->level() == level) {
    token::Token* token = tokens.consume();
    std::unique_ptr<ast::Expression> right = parse_binary(level + 1);
    left = std::make_unique<ast::BinaryOp>(std::move(left),
        token->parse_str(), std::move(right));
  }
  return left;
}

std::unique_ptr<ast::Expression> Parser::parse_primary() {
  token::Token* token = tokens.consume();
  if (token->get_type() == token::Type::punctuation) {
    return parse_parenthesis();
  }
  if (token->get_type() == token::Type::literal) {
      return std::make_unique<ast::Literal>(std::stoi(token->get_content()));
  }
  if (token->get_type() == token::Type::identifier) {
      return std::make_unique<ast::Identifier>(*token);
  }
  throw ParseException("Expected literal or identifier, got " + std::string(*token));
}
