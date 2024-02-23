#include "../parser.h"

#include "../tools/exceptions.h"
#include "../tools/readwrite.h"

#include "token.h"
#include "ast.h"


namespace token {

  Location::Location(int line, int column): line(line), column(column) {}

  Location::operator std::string() const {
    return "(" + std::to_string(line) + "," + std::to_string(column) + ")";
  }

  std::string to_string(const Type& type) {
    if (type == Type::whitespace) return "WHITESPACE";
    if (type == Type::left_parenthesis) return "LEFT_PARENTHESIS";
    if (type == Type::right_parenthesis) return "RIGHT_PARENTHESIS";
    if (type == Type::left_brace) return "LEFT_BRACE";
    if (type == Type::right_brace) return "RIGHT_BRACE";
    if (type == Type::dot) return "DOT";
    if (type == Type::comma) return "COMMA";
    if (type == Type::colon) return "COLON";
    if (type == Type::semicolon) return "SEMICOLON";
    if (type == Type::plus) return "PLUS";
    if (type == Type::minus) return "MINUS";
    if (type == Type::product) return "PRODUCT";
    if (type == Type::division) return "DIVISION";
    if (type == Type::modulo) return "MODULO";
    if (type == Type::bang) return "BANG";
    if (type == Type::equal) return "EQUAL";
    if (type == Type::equal_equal) return "EQUAL_EQUAL";
    if (type == Type::not_equal) return "NOT_EQUAL";
    if (type == Type::less) return "LESS";
    if (type == Type::less_or_equal) return "LESS_OR_EQUAL";
    if (type == Type::greater) return "GREATER";
    if (type == Type::greater_or_equal) return "GREATER_OR_EQUAL";
    if (type == Type::keyword) return "KEYWORD";
    if (type == Type::identifier) return "IDENTIFIER";
    if (type == Type::literal) return "LITERAL";
    if (type == Type::eof) return "EOF";
    return "";
  }

  std::string to_string(const int level) {
    if (level == expression) return "expression";
    if (level == term) return "term";
    if (level == factor) return "factor";
    if (level == primary) return "primary";
    if (level == unknown) return "unknown";
    return "";
  }

  Token::Token(Type type, std::string content, Location location):
    type(type), content(content), location(location) {}

  Token::operator std::string() const {
    std::string result = "Token(";
    result += content + ", ";
    result += to_string(type) + ", ";
    result += to_string(level()) + ", ";
    result += std::string(location) + ")";
    return result;
  }

  std::string Token::get_content() const {
    return content;
  }

  std::string Token::parse_str() {
    if (type == Type::identifier) return content;
    throw ParseException(message({ Type::identifier }));
  }

  bool Token::match(std::vector<Type> types) {
    for (Type match_type: types) {
      if (type == match_type) return true;
    }
    return false;
  }

  int Token::level() const {
    if (type == Type::identifier) return primary;
    if (type == Type::literal) return primary;
    if (content == "+" || content == "-") return term;
    if (content == "*" || content == "/") return factor;
    return unknown;
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
    return std::string(*this) + " is not " + types_to_string(need);
  }

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

} /* token */
