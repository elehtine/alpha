#include "tokeniser.h"
#include "tools/readwrite.h"


std::unique_ptr<Tokens> tokenise(Source& source, Printer* printer) {
  Tokeniser tokeniser(source, printer);
  return tokeniser.tokenise();
}

Tokeniser::Tokeniser(Source& source, Printer* printer):
  source(source),
  printer(printer) {}

std::unique_ptr<Tokens> Tokeniser::tokenise() {
  std::vector<std::unique_ptr<Token>> token_list;
  for (line = 1; line <= source.size(); line++) {
    column = 0;
    while (column < (int) source.line(line).size()) {
      std::unique_ptr<Token> token = scan_token();
      if (!token) continue;
      token_list.push_back(std::move(token));
    }
  }

  token_list.push_back(std::make_unique<Token>(token::Type::eof, "end",
        Location(source.size(), 0, "")));

  return std::make_unique<Tokens>(printer, token_list);
}

std::unique_ptr<Token> Tokeniser::scan_token() {
  last_column = column;

  if (check(whitespace) || check(comment)) {
    return std::unique_ptr<Token>(nullptr);
  }

  if (check(punctuation) || check(oper) || check(keyword)) {
    std::string content = source.line(line, last_column, column);
    if (!str_to_type.count(content)) {
      throw Location(line, column, source.line(line)).error();
    }
    return create_token(str_to_type[content], content);
  }

  if (check(identifier)) {
    std::string content = source.line(line, last_column, column);
    return create_token(token::Type::identifier, content);
  }
  if (check(literal)) {
    std::string content = source.line(line, last_column, column);
    return create_token(token::Type::literal, content);
  }

  Location location(line, column, source.line(line));
  throw location.error();
}

std::unique_ptr<Token> Tokeniser::create_token(token::Type type, std::string content) {
  Location location(line, last_column, source.line(line));
  return std::make_unique<Token>(type, content, location);
}

bool Tokeniser::check(const std::regex& expression) {
  std::string current = source.line(line).substr(column);
  std::smatch match;
  if (!std::regex_search(current, match, expression)) return false;
  column += match[0].length();
  return true;
}
