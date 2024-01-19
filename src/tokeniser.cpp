#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <iterator>
#include <utility>

#include "tokeniser.h"


namespace tokeniser {

  std::string to_string(Type type) {
    if (type == Type::literal) return "literal";
    if (type == Type::identifier) return "identifier";
    if (type == Type::eol) return "EOL";
    if (type == Type::end) return "END";
    return "";
  }

  Token::Token(Type type, std::string content):
    type(type), content(content) {}

  Token::operator std::string() const {
    std::string show = type == Type::eol ? "\\n" : content;
    return "Token(" + to_string(type) + ", " + show + ")";
  }

  void parse(const std::string& content,
      std::back_insert_iterator<std::vector<Token>> inserter,
      std::vector<std::pair<std::regex, Type>>::const_iterator types_begin,
      std::vector<std::pair<std::regex, Type>>::const_iterator types_end) {
    if (types_begin == types_end) return;

    std::regex regex = types_begin->first;
    auto lines_begin =
      std::sregex_iterator(content.begin(), content.end(), regex);
    auto lines_end = std::sregex_iterator();

    int last = 0;
    for (std::sregex_iterator i = lines_begin; i != lines_end; i++) {
      int pos = i->position();
      std::string between = content.substr(last, pos-last);
      parse(between, inserter, types_begin+1, types_end);
      inserter = Token(types_begin->second, i->str());
      last = pos+1;
    }
    std::string between = content.substr(last, content.size());
    parse(between, inserter, types_begin+1, types_end);
  }

  std::vector<Token> tokenise(const std::string& content) {
    std::vector<std::pair<std::regex, Type>> types = {
      { std::regex("\\n"), Type::eol },
      { std::regex("[+-]"), Type::identifier },
      { std::regex("\\d"), Type::literal },
    };
    std::vector<Token> tokens;
    parse(content, std::back_inserter(tokens), types.begin(), types.end());
    tokens.push_back(Token(Type::end, "END"));
    return tokens;
  }

} /* tokeniser */
