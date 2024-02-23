#include <string>
#include <vector>
#include <regex>
#include <iterator>
#include <utility>

#include "tokens.h"

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

token::Token* Tokens::peek() const {
  int index = std::min(position, tokens.size() - 1);
  return tokens[index].get();
}

token::Token* Tokens::consume() {
  token::Token* result = peek();
  if (position < tokens.size()) position++;
  return result;
}

bool Tokens::match(std::vector<token::Type> types) {
  token::Token* token = peek();
  if (!token->match(types)) return false;
  consume();
  return true;
}

void Tokens::tokenise() {
  for (line = 1; line <= source.size(); line++) {
    column = 0;
    while (column < (int) source.line(line).size()) {
      std::unique_ptr<token::Token> token = create_token();
      if (token) tokens.push_back(std::move(token));
    }
  }

  tokens.push_back(std::make_unique<token::Token>(token::Type::eof, "end",
        token::Location(source.size(), 0)));
}

std::unique_ptr<token::Token> Tokens::create_token() {
  int last = column;

  if (check(whitespace) || check(comment)) {
    return std::unique_ptr<token::Token>(nullptr);
  }

  if (check(punctuation)) {
    std::string content = source.line(line, last, column);
    return std::make_unique<token::Token>(str_to_type[content], content,
        token::Location(line, last));
  }
  if (check(oper)) {
    std::string content = source.line(line, last, column);
    return std::make_unique<token::Token>(str_to_type[content], content,
        token::Location(line, last));
  }
  if (check(identifier)) {
    std::string content = source.line(line, last, column);
    std::smatch match;
    if (std::regex_match(content, keyword)) {
      return std::make_unique<token::Token>(token::Type::keyword,
          content, token::Location(line, last));
    }
    return std::make_unique<token::Token>(token::Type::identifier, content,
        token::Location(line, last));
  }
  if (check(literal)) {
    std::string content = source.line(line, last, column);
    return std::make_unique<token::Token>(token::Type::literal, content,
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
