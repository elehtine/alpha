#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <iterator>
#include <utility>

#include "tokeniser.h"

#include "tools/readwrite.h"
#include "tools/exceptions.h"


namespace tokeniser {

  std::string to_string(const Type& type) {
    if (type == Type::eof) return "EOF";
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

  int Token::parse_int() {
    if (type == Type::literal) return stoi(content);
    throw ParseException(message({ Type::literal }));
  }

  std::string Token::parse_str() {
    if (type == Type::identifier) return content;
    if (type == Type::oper) return content;
    throw ParseException(message({ Type::oper, Type::identifier }));
  }

  std::string Token::message(std::vector<Type> need) {
    return "Token (" + std::string(*this) + ") is not " +
      ::to_string(need);
  }

  Tokeniser::Tokeniser(const std::string& content): content(content) {
    try {
      tokenise();
    } catch (const TokeniseException& exception) {
      error = std::string(exception.what()) + "\n";
    }
  }

  std::vector<Token> Tokeniser::get_tokens() const {
    return tokens;
  }

  Tokeniser::operator std::string() {
    std::string result = "";
    for (const Token& element: tokens) {
      result += std::string(element) + "\n";
    }
    result += error;
    return result;
  }

  void Tokeniser::tokenise() {
    while (position < content.size()) {
      bool unknown = true;
      for (auto p: types) {
        std::regex expression = p.first;
        Type type = p.second;
        if (check(expression, type)) {
          unknown = false;
          break;
        }
      }

      if (unknown) {
        throw TokeniseException(content.substr(position, 10));
      }
    }
  }

  bool Tokeniser::check(const std::regex& expression, const Type& type) {
    std::string current = content.substr(position);
    std::smatch match;
    if (!std::regex_search(current, match, expression)) return false;

    position += match[0].length();
    if (type == Type::whitespace) return true;

    tokens.push_back(Token(type, match[0]));
    return true;
  }

} /* tokeniser */
