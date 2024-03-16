#include "types/location.h"
#include "tokeniser.h"
#include "tools/files.h"
#include "tools/exceptions.h"


std::unique_ptr<Tokens> tokenise(Source& source, Printer* printer) {
  Tokeniser tokeniser(source, printer);
  return tokeniser.tokenise();
}

Tokeniser::Tokeniser(Source& source, Printer* printer):
  source(source),
  printer(printer)
{}

std::unique_ptr<Tokens> Tokeniser::tokenise() {
  std::vector<std::unique_ptr<Token>> token_list;
  for (line = 1; line <= source.size(); line++) {
    column = 0;
    while (column < (int) source.line(line).size()) {
      std::unique_ptr<Token> token = scan_token();
      if (!token) continue;
      if (!token_list.empty() && token_list.back()->match(TokenType::right_brace)) {
        bool brace = token->match(TokenType::right_brace);
        bool semicolon = token->match(TokenType::semicolon);
        bool keyword_else = token->match(TokenType::keyword_else);
        if (!brace && !semicolon && !keyword_else) {
          std::unique_ptr<Token> semicolon_token =
            std::make_unique<Token>(TokenType::semicolon, ";", token_list.back()->copy_location());
          token_list.push_back(std::move(semicolon_token));
        }
      }
      token_list.push_back(std::move(token));
    }
  }

  if (!token_list.empty() && token_list.back()->match(TokenType::right_brace)) {
    std::unique_ptr<Token> semicolon_token =
      std::make_unique<Token>(TokenType::semicolon, ";", token_list.back()->copy_location());
    token_list.push_back(std::move(semicolon_token));
  }

  token_list.push_back(std::make_unique<Token>(TokenType::eof, "end",
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
      Location location(line, column, source.line(line));
      std::string message = "Unknown tokens: '" + source.line(line, column, column+10) + "'";
      throw TokeniseException(location.error(message));
    }
    return create_token(str_to_type[content], content);
  }

  if (check(identifier)) {
    std::string content = source.line(line, last_column, column);
    return create_token(TokenType::identifier, content);
  }
  if (check(literal)) {
    std::string content = source.line(line, last_column, column);
    return create_token(TokenType::literal, content);
  }

  Location location(line, column, source.line(line));
  std::string message = "Unknown tokens: '" + source.line(line, column, column + 10) + "'";
  throw TokeniseException(location.error(message));
}

std::unique_ptr<Token> Tokeniser::create_token(TokenType type, std::string content) {
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
