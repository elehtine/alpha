#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <iterator>

#include "tokeniser.h"
#include "readwrite.h"


namespace tokeniser {

  Token::Token(Type type, std::string content):
    type(type), content(content) {}

  std::string Token::to_string() const {
    return content;
  }

  void parse(const std::string& content,
      std::back_insert_iterator<std::vector<Token>> inserter,
      std::vector<std::regex>::const_iterator regex_begin,
      std::vector<std::regex>::const_iterator regex_end,
      std::vector<Type>::const_iterator types_begin,
      std::vector<Type>::const_iterator types_end) {
    if (regex_begin == regex_end) return;

    auto lines_begin =
      std::sregex_iterator(content.begin(), content.end(), *regex_begin);
    auto lines_end = std::sregex_iterator();

    int last = 0;
    for (std::sregex_iterator i = lines_begin; i != lines_end; i++) {
      int pos = i->position();
      std::string between = content.substr(last, pos-last);
      parse(between, inserter, regex_begin+1, regex_end,
          types_begin+1, types_end);
      inserter = Token(*types_begin, i->str());
      last = pos+1;
    }
    std::string between = content.substr(last, content.size());
    parse(between, inserter, regex_begin+1, regex_end,
        types_begin+1, types_end);
  }

  void tokenise(const std::string& filename) {
    const std::string input_extension = ".alpha";
    const std::string output_extension = ".tokens.output";

    std::cout << "tokenise: " << filename << std::endl;
    std::string content = read(filename + input_extension);

    std::vector<std::regex> expressions = {
      std::regex("\\n"),
      std::regex("[+-]"),
      std::regex("\\d"),
    };
    std::vector<Type> types = {
      Type::eol,
      Type::identifier,
      Type::literal,
    };

    std::vector<Token> tokens;
    parse(content, std::back_inserter(tokens),
        expressions.begin(),
        expressions.end(),
        types.begin(),
        types.end());
    tokens.push_back(Token(Type::end, "END"));

    std::string result = "";
    for (const Token& token: tokens) {
      result += token.to_string();
    }

    write(filename + output_extension, result);
  }

} /* tokeniser */
