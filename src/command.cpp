#include <string>
#include <vector>
#include <iostream>

#include "command.h"
#include "tokeniser.h"
#include "readwrite.h"


Command::~Command() {}

Command::Command() {}

void Command::execute() {}

bool Command::check(int argc, char* argv[]) { 
  return false;
}

std::string to_string(std::vector<tokeniser::Token> tokens) {
  std::string result = "";
  for (const tokeniser::Token& token: tokens) {
    result += std::string(token) + "\n";
  }
  return result;
}

void Test::execute() {
  const std::string input_extension = ".alpha";
  const std::string output_extension = ".tokens.output";

  std::vector<std::string> files {
    "tests/addition",
    "tests/substitution"
  };

  for (const std::string& file: files) {
    std::string content = read(file + input_extension);
    std::vector<tokeniser::Token> tokens = tokeniser::tokenise(content);
    write(file + output_extension, to_string(tokens));
  }

}

bool Test::check(int argc, char* argv[]) {
  return argc == 2 && std::string(argv[1]) == "test";
}

void Compile::execute() {
  std::string content = read(filename);
  std::vector<tokeniser::Token> tokens = tokeniser::tokenise(content);
  std::cout << to_string(tokens) << std::endl;
}

bool Compile::check(int argc, char* argv[]) {
  if (argc != 3) return false;
  filename = argv[2];
  return true;
}

void Help::execute() {
  std::cout << "Usage:" << std::endl;
  std::cout << "Run compiler: alpha run <file>" << std::endl;
  std::cout << "Run tests: alpha test" << std::endl;
}

bool Help::check(int argc, char* argv[]) {
  return true;
}
