#include <string>
#include <iostream>

#include "test.h"
#include "tools/readwrite.h"
#include "tools/exceptions.h"

#include "tokeniser.h"
#include "parser.h"
#include "compiler.h"


enum class FileType {
  source,
  tokens,
  tree,
  interpret,
};

std::string filename(const std::string& file, const FileType type) {
  std::string suffix = ".unknown";
  if (type == FileType::source) suffix = ".alpha";
  if (type == FileType::tokens) suffix = ".tokens";
  if (type == FileType::tree) suffix = ".tree";
  if (type == FileType::interpret) suffix = ".interpret";
  return file + suffix;
}

void check_result(const std::string& in_file, const std::string& out_file,
    const std::string& input, const std::string& output) {
  bool write_to_file = true;
  if (is_file(out_file)) {
    std::string correct = read(out_file);
    if (correct == output) {
      std::cout << "File passed: " << in_file << std::endl;
      return;
    }

    std::cout << "File failed: " << in_file << std::endl;
    std::cout << "Input:" << std::endl;
    std::cout << input << std::endl;
    std::cout << "Output:" << std::endl;
    std::cout << output << std::endl;
    std::cout << "Correct:" << std::endl;
    std::cout << correct << std::endl;
    write_to_file = user_approval("Do you want to override [Y/n]? ");
  }

  if (!write_to_file) return;

  write(out_file, output);
  std::cout << "File saved: " << out_file << std::endl;
}

void test_file(const std::string& file) {
  if (!is_file(filename(file, FileType::source))) {
    std::cout << "File doesn't exists: ";
    std::cout << filename(file, FileType::source) << std::endl;
    return;
  }

  std::string source = read(filename(file, FileType::source));
  compiler::Compiler compiler(source);

  check_result(filename(file, FileType::source), filename(file, FileType::tokens),
      source, compiler.tokens(false));
  check_result(filename(file, FileType::tokens), filename(file, FileType::tree),
      compiler.tokens(false), compiler.tree(false));
  check_result(filename(file, FileType::tree), filename(file, FileType::interpret),
      compiler.tree(false), compiler.interpret(false));
}
