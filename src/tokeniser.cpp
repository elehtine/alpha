#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <iterator>
#include <utility>

#include "tokeniser.h"

#include "tools/readwrite.h"
#include "tools/exceptions.h"


Tokeniser::Tokeniser(const std::string& source, Printer& printer
    ): source(source) {
  position = 0;
  tokenise();

  printer.print_tokens(tokens);
}

std::vector<token::Token> Tokeniser::get_tokens() {
  return tokens;
}

void Tokeniser::tokenise() {
  while (position < source.size()) {
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
      throw TokeniseException(source.substr(position, 10));
    }
  }
}

bool Tokeniser::check(const std::regex& expression,
    const token::Type& type) {
  std::string current = source.substr(position);
  std::smatch match;
  if (!std::regex_search(current, match, expression)) return false;

  position += match[0].length();
  if (type == token::Type::whitespace) return true;

  tokens.push_back(token::Token(type, match[0]));
  return true;
}
