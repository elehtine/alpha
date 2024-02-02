#include <string>
#include <vector>
#include <iostream>
#include <regex>

#include "command.h"
#include "tools/readwrite.h"

#include "tokeniser.h"
#include "parser.h"
#include "compiler.h"


Command::~Command() {}

Command::Command() {}

void Test::execute() {
  std::vector<std::string> files = test_files();
  std::regex file_expression("(tests/\\w+)\\.alpha");

  for (const std::string& file: files) {
    std::smatch match;
    if (!std::regex_match(file, match, file_expression)) continue;
    std::string name = match[1];
    std::string source = read(name + ".alpha");

    Compiler compiler(source, false);
    FilePrinter printer(name);
    compiler.compile(printer);
  }
}

bool Test::check(int argc, char* argv[]) {
  return argc == 2 && std::string(argv[1]) == "test";
}

void Compile::execute() {
  if (!is_file(filename)) {
    std::cout << "File doesn't exist: " << filename << std::endl;
    return;
  }

  std::string source = read(filename);
  Compiler compiler(source, true);
  UserPrinter printer;
  compiler.compile(printer);
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
