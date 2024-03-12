#include <vector>
#include <regex>

#include "command.h"
#include "tools/files.h"
#include "tools/printer.h"
#include "tools/exceptions.h"

#include "types/source.h"
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
    std::string content = read(name + ".alpha");

    std::unique_ptr<FilePrinter> printer = std::make_unique<FilePrinter>(name);
    Source source(content, printer.get());

    try {
      Compiler compiler(source, printer.get());
    } catch (const CompileException& exception) {
      printer->print_exception(exception);
    }
  }
}

bool Test::check(int argc, char* argv[]) {
  return argc == 2 && std::string(argv[1]) == "test";
}

void Compile::execute() {
  std::unique_ptr<UserPrinter> printer = std::make_unique<UserPrinter>();

  if (!is_file(filename)) {
    printer->print("File doesn't exist: " + filename);
    return;
  }

  Source source(read(filename), printer.get());

  try {
    Compiler compiler(source, printer.get());
  } catch (const CompileException& exception) {
    printer->print_exception(exception);
  }
}

bool Compile::check(int argc, char* argv[]) {
  if (argc != 3) return false;
  filename = argv[2];
  return true;
}

void Help::execute() {
  std::unique_ptr<UserPrinter> printer = std::make_unique<UserPrinter>();
  printer->print("Usage:");
  printer->print("Run compiler: alpha run <file>");
  printer->print("Run tests: alpha test");
}

bool Help::check(int argc, char* argv[]) {
  return true;
}
