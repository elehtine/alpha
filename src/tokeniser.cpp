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

  void literals(const std::string& content,
      std::back_insert_iterator<std::vector<Token>> inserter) {
    std::regex regex("\\d");
    auto lines_begin =
      std::sregex_iterator(content.begin(), content.end(), regex);
    auto lines_end = std::sregex_iterator();

    for (std::sregex_iterator i = lines_begin; i != lines_end; i++) {
      inserter = Token(Type::literal, i->str());
    }
  }

  void identifiers(const std::string& content,
      std::back_insert_iterator<std::vector<Token>> inserter) {
    std::vector<Token> tokens;

    std::regex regex("[+-]");
    auto lines_begin =
      std::sregex_iterator(content.begin(), content.end(), regex);
    auto lines_end = std::sregex_iterator();

    int last = 0;
    for (std::sregex_iterator i = lines_begin; i != lines_end; i++) {
      int pos = i->position();
      std::string before = content.substr(last, pos-last);
      literals(before, inserter);
      inserter = Token(Type::identifier, i->str());
      last = pos+1;
    }

    std::string before = content.substr(last, content.size());
    literals(before, inserter);
  }

  void lines(const std::string& content,
      std::back_insert_iterator<std::vector<Token>> inserter) {
    std::regex regex("\\n");
    auto lines_begin =
      std::sregex_iterator(content.begin(), content.end(), regex);
    auto lines_end = std::sregex_iterator();

    int last = 0;
    for (std::sregex_iterator i = lines_begin; i != lines_end; i++) {
      int pos = i->position();
      std::string line = content.substr(last, pos-last);
      identifiers(line, inserter);
      inserter = Token(Type::eol, "\n");
      last = pos+1;
    }
    inserter = Token(Type::end, "END");
  }

  void tokenise(const std::string& filename) {
    const std::string input_extension = ".alpha";
    const std::string output_extension = ".tokens.output";

    std::cout << "tokenise: " << filename << std::endl;
    std::string content = read(filename + input_extension);

    std::vector<Token> tokens;
    lines(content, std::back_inserter(tokens));

    std::string result = "";
    for (const Token& token: tokens) {
      result += token.to_string();
    }

    write(filename + output_extension, result);
  }

} /* tokeniser */
