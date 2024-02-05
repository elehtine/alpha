#include "../tools/exceptions.h"
#include "../tools/readwrite.h"

#include "token.h"
#include "ast.h"


namespace token {

  std::string to_string(const Type& type) {
    if (type == Type::eof) return "EOF";
    if (type == Type::punctuation) return "punctuation";
    if (type == Type::oper) return "operator";
    if (type == Type::identifier) return "identifier";
    if (type == Type::literal) return "literal";
    return "";
  }

  Token::Token(Type type, std::string content):
    type(type), content(content) {}

  Token::operator std::string() const {
    return "Token(" + to_string(type) + ", " + content + ")";
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

  std::unique_ptr<ast::Expression> Token::parse() const {
    if (type == token::Type::literal) {
      return std::make_unique<ast::Literal>(stoi(content));
    }
    if (type == token::Type::identifier) {
      return std::make_unique<ast::Identifier>(content);
    }
    throw ParseException("Excpected parenthesis, literal or identifier" +
        std::string(*this));
  }

  std::string types_to_string(std::vector<Type> vec) {
    std::string result = "";
    for (const Type& element: vec) {
      result += to_string(element) + "\n";
    }
    return result;
  }

  std::string Token::message(std::vector<Type> need) const {
    return "Token (" + std::string(*this) + ") is not " +
      types_to_string(need);
  }

} /* token */
