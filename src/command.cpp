#include <string>
#include <vector>
#include <iostream>

#include "command.h"
#include "tools/readwrite.h"
#include "test.h"

#include "tokeniser.h"
#include "parser.h"


Command::~Command() {}

Command::Command() {}

void Test::execute() {
  std::vector<std::string> files {
    "tests/arithmetic",
  };

  for (const std::string& file: files) {
    test_file(file);
  }
}

bool Test::check(int argc, char* argv[]) {
  return argc == 2 && std::string(argv[1]) == "test";
}

void Compile::execute() {
  std::string content = read(filename);
  std::vector<tokeniser::Token> tokens = tokeniser::tokenise(content);
  std::cout << to_string(tokens) << std::endl;

  parser::Parser parser { tokens };
  std::unique_ptr<parser::Expression> expression = parser.parse();
  std::cout << std::string(*expression) << std::endl;
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
