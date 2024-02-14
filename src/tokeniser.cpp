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
    ): source(escape(source)) {
  position = 0;
  printer.print_source(source);

  tokenise();
  printer.print_tokens(get_tokens());
}

std::vector<token::Token*> Tokeniser::get_tokens() {
  std::vector<token::Token*> result;
  for (std::unique_ptr<token::Token>& token: tokens) {
    result.push_back(token.get());
  }
  return result;
}

std::string Tokeniser::escape(const std::string& content) {
  std::string result = "";
  for (std::size_t index = 0; index < content.size(); index++) {
    if (content[index] == '\n') result += "\\n";
    else result += content[index];
  }
  return result;
}

void Tokeniser::tokenise() {
  while (position < source.size()) {
    std::unique_ptr<token::Token> token = create_token();
    if (token) tokens.push_back(std::move(token));
  }
  tokens.push_back(std::make_unique<token::Eof>(token::Type::eof, "end"));
}

std::unique_ptr<token::Token> Tokeniser::create_token() {
  std::size_t last = position;
  if (check(token::Whitespace::expression)) {
    return std::unique_ptr<token::Whitespace>(nullptr);
  }
  if (check(token::Punctuation::expression)) {
    std::string content = source.substr(last, position - last);
    return std::make_unique<token::Punctuation>(token::Type::punctuation, content);
  }
  if (check(token::Oper::expression)) {
    std::string content = source.substr(last, position - last);
    return std::make_unique<token::Oper>(token::Type::oper, content);
  }
  if (check(token::Identifier::expression)) {
    std::string content = source.substr(last, position - last);
    return std::make_unique<token::Identifier>(token::Type::identifier, content);
  }
  if (check(token::Literal::expression)) {
    std::string content = source.substr(last, position - last);
    return std::make_unique<token::Literal>(token::Type::literal, content);
  }

  throw TokeniseException(source.substr(position, 10));
}

bool Tokeniser::check(const std::regex& expression) {
  std::string current = source.substr(position);
  std::smatch match;
  if (!std::regex_search(current, match, expression)) return false;
  position += match[0].length();
  return true;
}
