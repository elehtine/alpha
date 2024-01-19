#include <iostream>
#include <string>
#include <vector>

#include "src/tokeniser.h"


void help() {
  std::cout << "Usage:" << std::endl;
  std::cout << "Run compiler: alpha run <file>" << std::endl;
  std::cout << "Run tests: alpha test" << std::endl;
}

void run(std::string file) {
  std::cout << "file: " << file << std::endl;
  tokeniser::tokenise(file);
}

void test() {
  std::vector<std::string> files {
    "tests/addition",
    "tests/substitution"
  };

  for (const std::string& file: files) {
    tokeniser::tokenise(file);
  }
}

int main(int argc, char* argv[]) {
  std::cout << "args:" << std::endl;
  for (int i = 0; i < argc; i++) {
    std::cout << "arg " << i << ": " << argv[i] << std::endl;
  }
  std::cout << std::endl;

  if (argc < 2) {
    help();
    return 0;
  }

  std::string command = argv[1];
  if (command == "run" && argc < 3) {
    help();
    return 0;
  }

  if (command == "run") {
    std::string file = argv[2];
    run(file);
  }
  if (command == "test") {
    test();
  }
}
