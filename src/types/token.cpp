#include "../parser.h"

#include "../tools/exceptions.h"
#include "../tools/readwrite.h"

#include "token.h"
#include "ast.h"


std::string to_string(const TokenType& type) {
  if (type == TokenType::left_parenthesis) return "LEFT_PARENTHESIS";
  if (type == TokenType::right_parenthesis) return "RIGHT_PARENTHESIS";
  if (type == TokenType::left_brace) return "LEFT_BRACE";
  if (type == TokenType::right_brace) return "RIGHT_BRACE";
  if (type == TokenType::dot) return "DOT";
  if (type == TokenType::comma) return "COMMA";
  if (type == TokenType::colon) return "COLON";
  if (type == TokenType::semicolon) return "SEMICOLON";
  if (type == TokenType::plus) return "PLUS";
  if (type == TokenType::minus) return "MINUS";
  if (type == TokenType::product) return "PRODUCT";
  if (type == TokenType::division) return "DIVISION";
  if (type == TokenType::modulo) return "MODULO";
  if (type == TokenType::keyword_not) return "NOT";
  if (type == TokenType::equal) return "EQUAL";
  if (type == TokenType::equal_equal) return "EQUAL_EQUAL";
  if (type == TokenType::not_equal) return "NOT_EQUAL";
  if (type == TokenType::less) return "LESS";
  if (type == TokenType::less_or_equal) return "LESS_OR_EQUAL";
  if (type == TokenType::greater) return "GREATER";
  if (type == TokenType::greater_or_equal) return "GREATER_OR_EQUAL";
  if (type == TokenType::logical_and) return "LOGICAL_AND";
  if (type == TokenType::logical_or) return "LOGICAL_OR";
  if (type == TokenType::var) return "KEYWORD";
  if (type == TokenType::integer) return "INTEGER";
  if (type == TokenType::boolean) return "BOOLEAN";
  if (type == TokenType::keyword_true) return "TRUE";
  if (type == TokenType::keyword_false) return "FALSE";
  if (type == TokenType::keyword_if) return "IF";
  if (type == TokenType::keyword_then) return "THEN";
  if (type == TokenType::keyword_else) return "ELSE";
  if (type == TokenType::keyword_while) return "WHILE";
  if (type == TokenType::keyword_do) return "DO";
  if (type == TokenType::identifier) return "IDENTIFIER";
  if (type == TokenType::literal) return "LITERAL";
  if (type == TokenType::eof) return "EOF";
  return "";
}

std::string to_string(const int level) {
  if (level == expression) return "expression";
  if (level == assignment) return "assignment";
  if (level == logical_or) return "logical_or";
  if (level == logical_and) return "logical_and";
  if (level == equality) return "equality";
  if (level == comparison) return "comparison";
  if (level == term) return "term";
  if (level == factor) return "factor";
  if (level == primary) return "primary";
  if (level == unknown) return "unknown";
  return "";
}

Token::Token(TokenType type, std::string content, Location location):
  type(type), content(content), location(location)
{}

Token::operator std::string() const {
  std::string result = "Token(";
  result += content + ", ";
  result += to_string(type) + ", ";
  result += to_string(level()) + ", ";
  result += std::string(location) + ")";
  return result;
}

std::string Token::get_content() const {
  return content;
}

std::string Token::parse_str() {
  if (type == TokenType::identifier) return content;
  throw error({ TokenType::identifier });
}

Location Token::copy_location() const {
  return location;
}

bool Token::match(TokenType match_type) {
  return type == match_type;
}

int Token::level() const {
  if (content == "=") return assignment;
  if (content == "or") return logical_or;
  if (content == "and") return logical_and;
  if (content == "==" || content == "!=") return equality;
  if (content[0] == '<' || content[0] == '>') return comparison;
  if (content == "+" || content == "-") return term;
  if (content == "*" || content == "/" || content == "%") return factor;
  if (type == TokenType::identifier) return primary;
  if (type == TokenType::literal) return primary;
  return unknown;
}

ParseException Token::error(std::vector<TokenType> types) {
  std::string message = "expected ";
  for (TokenType type: types) {
    message += to_string(type) + ", ";
  }
  message += "got " + to_string(type) + "\n";
  message += location.error_mark() + "unexpected token";
  return ParseException(message);
}

std::string types_to_string(std::vector<TokenType> vec) {
  std::string result = "";
  bool inter;
  for (const TokenType& element: vec) {
    if (inter) result += " or ";
    inter = true;
    result += to_string(element);
  }
  return result;
}

Tokens::Tokens(Printer* printer, std::vector<std::unique_ptr<Token>>& tokens):
  printer(printer), tokens(std::move(tokens)) {
    printer->print_tokens(get_tokens());
  };

std::vector<Token*> Tokens::get_tokens() {
  std::vector<Token*> result;
  for (std::unique_ptr<Token>& token: tokens) {
    result.push_back(token.get());
  }
  return result;
}

Token* Tokens::previous() const {
  int index = std::min(position - 1, tokens.size() - 1);
  index = std::max(index, 0);
  return tokens[index].get();
}

Token* Tokens::peek() const {
  int index = std::min(position, tokens.size() - 1);
  return tokens[index].get();
}

Token* Tokens::peek_second() const {
  int index = std::min(position+1, tokens.size() - 1);
  return tokens[index].get();
}

Token* Tokens::consume() {
  Token* result = peek();
  if (position < tokens.size()) position++;
  return result;
}

bool Tokens::match(TokenType match_type) {
  Token* token = peek();
  if (!token->match(match_type)) return false;
  consume();
  return true;
}

bool Tokens::match(std::vector<TokenType> types) {
  for (TokenType match_type: types) {
    if (match(match_type)) return true;
  }
  return false;
}

ParseException Tokens::error(std::vector<TokenType> types) {
  return peek()->error(types);
}

ParseException Tokens::error(TokenType type) {
  return error(std::vector<TokenType>{ type });
}


