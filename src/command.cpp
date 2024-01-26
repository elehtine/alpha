#include <string>
#include <vector>
#include <iostream>

#include "command.h"
#include "tokeniser.h"
#include "readwrite.h"


Command::~Command() {}

Command::Command() {}

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
  const std::string test_extension = ".tokens.test";

  std::vector<std::string> files {
    "tests/addition",
    "tests/substitution",
    "tests/operators",
    "tests/identifier",
    "foo"
  };

  for (const std::string& file: files) {
    const std::string input_file = file + input_extension;
    const std::string output_file = file + output_extension;
    const std::string test_file = file + test_extension;

    if (!is_file(file + input_extension)) {
      std::cout << "File " << input_file << " doesn't exists" << std::endl;
      continue;
    }

    std::string content = read(input_file);
    std::vector<tokeniser::Token> tokens = tokeniser::tokenise(content);
    std::string result = to_string(tokens);
    write(output_file, result);

    if (is_file(test_file)) {
      std::string test_content = read(test_file);
      if (test_content == result) {
        std::cout << "File " << file << " passed" << std::endl;
      } else {
        std::cout << "File " << file << " failed" << std::endl;
      }
    } else {
      write(test_file, result);
        std::cout << "File " << file << " saved" << std::endl;
    }
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
