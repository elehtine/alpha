#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <iterator>
#include <utility>

#include "tokeniser.h"

#include "tools/readwrite.h"
#include "tools/exceptions.h"

Tokeniser::Tokeniser(const std::string& content): content(content) {
  try {
    tokenise();
  } catch (const TokeniseException& exception) {
    error = std::string(exception.what()) + "\n";
  }
}

std::vector<token::Token> Tokeniser::get_tokens() const {
  return tokens;
}

std::string Tokeniser::prefix() const {
  return "Tokens:\n";
}

Tokeniser::operator std::string() {
  std::string result = "";
  for (const token::Token& element: tokens) {
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
      token::Type type = p.second;
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

bool Tokeniser::check(const std::regex& expression,
    const token::Type& type) {
  std::string current = content.substr(position);
  std::smatch match;
  if (!std::regex_search(current, match, expression)) return false;

  position += match[0].length();
  if (type == token::Type::whitespace) return true;

  tokens.push_back(token::Token(type, match[0]));
  return true;
}
