#include "../parser.h"

#include "../tools/exceptions.h"
#include "../tools/readwrite.h"

#include "token.h"
#include "ast.h"


namespace token {

  Location::Location(int line, int column): line(line), column(column) {}

  Location::operator std::string() const {
    return "(" + std::to_string(line) + "," + std::to_string(column) + ")";
  }

  std::string to_string(const Type& type) {
    if (type == Type::whitespace) return "whitespace";
    if (type == Type::comment) return "comment";
    if (type == Type::punctuation) return "punctuation";
    if (type == Type::oper) return "operator";
    if (type == Type::identifier) return "identifier";
    if (type == Type::keyword) return "keyword";
    if (type == Type::literal) return "literal";
    if (type == Type::eof) return "EOF";
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

  Token::Token(Type type, std::string content, Location location):
    type(type), content(content), location(location) {}

  Token::operator std::string() const {
    std::string result = "Token(";
    result += content + ", ";
    result += to_string(type) + ", ";
    result += to_string(level()) + ", ";
    result += std::string(location) + ")";
    return result;
  }

  Type Token::get_type() const {
    return type;
  }

  std::string Token::get_content() const {
    return content;
  }

  std::string Token::parse_str() {
    if (type == Type::identifier) return content;
    if (type == Type::oper) return content;
    throw ParseException(message({ Type::oper, Type::identifier }));
  }

  std::unique_ptr<ast::Expression> Token::parse(Parser* parser) const {
    throw ParseException(message({ Type::literal, Type::identifier }));
  }

  int Token::level() const {
    if (type == Type::identifier) return primary;
    if (type == Type::literal) return primary;
    if (content == "+" || content == "-") return term;
    if (content == "*" || content == "/") return factor;
    return unknown;
  }

  std::string types_to_string(std::vector<Type> vec) {
    std::string result = "";
    bool inter;
    for (const Type& element: vec) {
      if (inter) result += " or ";
      inter = true;
      result += to_string(element);
    }
    return result;
  }

  std::string Token::message(std::vector<Type> need) const {
    return std::string(*this) + " is not " + types_to_string(need);
  }


} /* token */
