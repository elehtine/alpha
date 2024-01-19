#include <iostream>
#include <string>
#include <vector>

#include "tokeniser.h"
#include "readwrite.h"


namespace tokeniser {

  Token::Token(Type type, std::string content):
    type(type), content(content) {}

  std::vector<Token> tokenise(const std::string& filename) {
    const std::string input_extension = ".alpha";
    const std::string output_extension = ".tokens.output";

    std::cout << "tokenise: " << filename << std::endl;
    std::string content = read(filename + input_extension);
    write(filename + output_extension, content);

    std::vector<Token> tokens;

    tokens.push_back(Token(Type::end, ""));
    return tokens;
  }

}
