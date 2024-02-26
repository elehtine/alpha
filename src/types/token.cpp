#include "../parser.h"

#include "../tools/exceptions.h"
#include "../tools/readwrite.h"

#include "token.h"
#include "ast.h"


Location::Location(int row, int column, std::string line):
  row(row), column(column), line(line) {}

Location::operator std::string() const {
  return "(" + std::to_string(row) + "," + std::to_string(column) + ")";
}

std::string to_string(const token::Type& type) {
  if (type == token::Type::whitespace) return "WHITESPACE";
  if (type == token::Type::left_parenthesis) return "LEFT_PARENTHESIS";
  if (type == token::Type::right_parenthesis) return "RIGHT_PARENTHESIS";
  if (type == token::Type::left_brace) return "LEFT_BRACE";
  if (type == token::Type::right_brace) return "RIGHT_BRACE";
  if (type == token::Type::dot) return "DOT";
  if (type == token::Type::comma) return "COMMA";
  if (type == token::Type::colon) return "COLON";
  if (type == token::Type::semicolon) return "SEMICOLON";
  if (type == token::Type::plus) return "PLUS";
  if (type == token::Type::minus) return "MINUS";
  if (type == token::Type::product) return "PRODUCT";
  if (type == token::Type::division) return "DIVISION";
  if (type == token::Type::modulo) return "MODULO";
  if (type == token::Type::keyword_not) return "NOT";
  if (type == token::Type::equal) return "EQUAL";
  if (type == token::Type::equal_equal) return "EQUAL_EQUAL";
  if (type == token::Type::not_equal) return "NOT_EQUAL";
  if (type == token::Type::less) return "LESS";
  if (type == token::Type::less_or_equal) return "LESS_OR_EQUAL";
  if (type == token::Type::greater) return "GREATER";
  if (type == token::Type::greater_or_equal) return "GREATER_OR_EQUAL";
  if (type == token::Type::var) return "KEYWORD";
  if (type == token::Type::integer) return "INTEGER";
  if (type == token::Type::boolean) return "BOOLEAN";
  if (type == token::Type::keyword_true) return "TRUE";
  if (type == token::Type::keyword_false) return "FALSE";
  if (type == token::Type::keyword_if) return "IF";
  if (type == token::Type::keyword_then) return "THEN";
  if (type == token::Type::keyword_else) return "ELSE";
  if (type == token::Type::keyword_while) return "WHILE";
  if (type == token::Type::keyword_do) return "DO";
  if (type == token::Type::read_int) return "READ_INT";
  if (type == token::Type::print_int) return "PRINT_INT";
  if (type == token::Type::identifier) return "IDENTIFIER";
  if (type == token::Type::literal) return "LITERAL";
  if (type == token::Type::eof) return "EOF";
  return "";
}

std::string to_string(const int level) {
  if (level == expression) return "expression";
  if (level == term) return "term";
  if (level == factor) return "factor";
  if (level == primary) return "primary";
  if (level == unknown) return "unknown";
  return "";
}

Token::Token(token::Type type, std::string content, Location location):
  type(type), content(content), location(location) {}

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
  if (type == token::Type::identifier) return content;
  throw ParseException(message({ token::Type::identifier }));
}

bool Token::match(token::Type match_type) {
  return type == match_type;
}

int Token::level() const {
  if (content == "==" || content == "!=") return equality;
  if (content[0] == '<' || content[0] == '>') return comparison;
  if (content == "+" || content == "-") return term;
  if (content == "*" || content == "/" || content == "%") return factor;
  if (type == token::Type::identifier) return primary;
  if (type == token::Type::literal) return primary;
  return unknown;
}

std::string types_to_string(std::vector<token::Type> vec) {
  std::string result = "";
  bool inter;
  for (const token::Type& element: vec) {
    if (inter) result += " or ";
    inter = true;
    result += to_string(element);
  }
  return result;
}

std::string Token::message(std::vector<token::Type> need) const {
  return std::string(*this) + " is not " + types_to_string(need);
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

Token* Tokens::consume() {
  Token* result = peek();
  if (position < tokens.size()) position++;
  return result;
}

bool Tokens::match(token::Type match_type) {
  Token* token = peek();
  if (!token->match(match_type)) return false;
  consume();
  return true;
}

bool Tokens::match(std::vector<token::Type> types) {
  for (token::Type match_type: types) {
    if (match(match_type)) return true;
  }
  return false;
}
