#include <string>
#include <vector>
#include <regex>
#include <iterator>
#include <utility>

#include "tokeniser.h"

#include "tools/readwrite.h"
#include "tools/exceptions.h"


Tokens::Tokens(const Source& source, Printer* printer
    ): source(source), printer(printer) {
  tokenise();
  printer->print_tokens(get_tokens());
}

std::vector<token::Token*> Tokens::get_tokens() {
  std::vector<token::Token*> result;
  for (std::unique_ptr<token::Token>& token: tokens) {
    result.push_back(token.get());
  }
  return result;
}

void Tokens::tokenise() {
  for (line = 1; line <= source.size(); line++) {
    column = 0;
    while (column < (int) source.line(line).size()) {
      std::unique_ptr<token::Token> token = create_token();
      if (token) tokens.push_back(std::move(token));
    }
  }

  tokens.push_back(std::make_unique<token::Eof>(token::Type::eof, "end",
        token::Location(source.size(), 0)));
}

std::unique_ptr<token::Token> Tokens::create_token() {
  int last = column;

  if (check(token::Whitespace::expression)) {
    return std::unique_ptr<token::Whitespace>(nullptr);
  }

  if (check(token::Comment::expression)) {
    std::string content = source.line(line, last, column);
    return std::make_unique<token::Comment>(token::Type::punctuation, content,
        token::Location(line, last));
  }

  if (check(token::Punctuation::expression)) {
    std::string content = source.line(line, last, column);
    return std::make_unique<token::Punctuation>(token::Type::punctuation, content,
        token::Location(line, last));
  }
  if (check(token::Oper::expression)) {
    std::string content = source.line(line, last, column);
    return std::make_unique<token::Oper>(token::Type::oper, content,
        token::Location(line, last));
  }
  if (check(token::Identifier::expression)) {
    std::string content = source.line(line, last, column);
    return std::make_unique<token::Identifier>(token::Type::identifier, content,
        token::Location(line, last));
  }
  if (check(token::Literal::expression)) {
    std::string content = source.line(line, last, column);
    return std::make_unique<token::Literal>(token::Type::literal, content,
        token::Location(line, last));
  }

  throw TokeniseException(source.line(line, column, column + 10));
}

bool Tokens::check(const std::regex& expression) {
  std::string current = source.line(line).substr(column);
  std::smatch match;
  if (!std::regex_search(current, match, expression)) return false;
  column += match[0].length();
  return true;
}
