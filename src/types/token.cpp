#include "../tools/exceptions.h"
#include "../tools/readwrite.h"

#include "token.h"
#include "ast.h"


namespace token {

  std::string to_string(const Type& type) {
    if (type == Type::whitespace) return "whitespace";
    if (type == Type::punctuation) return "punctuation";
    if (type == Type::oper) return "operator";
    if (type == Type::identifier) return "identifier";
    if (type == Type::literal) return "literal";
    if (type == Type::eof) return "EOF";
    return "";
  }

  Token::Token(Type type, std::string content):
    type(type), content(content) {}

  Token::operator std::string() const {
    return "Token(" + to_string(type) + ", '" + content + "')";
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
    throw ParseException(message({ Type::literal, Type::identifier }));
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
    return "Token (" + std::string(*this) + ") is not " +
      types_to_string(need);
  }

  Whitespace::Whitespace(Type type, std::string content):
    Token(type, content) {}
  Punctuation::Punctuation(Type type, std::string content):
    Token(type, content) {}
  Oper::Oper(Type type, std::string content):
    Token(type, content) {}
  Identifier::Identifier(Type type, std::string content):
    Token(type, content) {}
  Literal::Literal(Type type, std::string content):
    Token(type, content) {}
  Eof::Eof(Type type, std::string content):
    Token(type, content) {}

  const std::regex Whitespace::expression { "^[\\s|\\\\n]+" };
  const std::regex Punctuation::expression { "^(\\(|\\))" };
  const std::regex Oper::expression { "^(\\+|-|\\*|/)" };
  const std::regex Identifier::expression { "^[a-zA-Z]\\w*" };
  const std::regex Literal::expression { "^\\d+" };

} /* token */
