#include <string>
#include <iostream>

#include "test.h"
#include "tools/readwrite.h"
#include "tools/exceptions.h"

#include "tokeniser.h"
#include "parser.h"
#include "compiler.h"


std::string alpha_file(const std::string& file) {
  const std::string input_extension = ".alpha";
  return file + input_extension;
}

std::string token_file(const std::string& file) {
  const std::string output_extension = ".tokens";
  return file + output_extension;
}

std::string tree_file(const std::string& file) {
  const std::string output_extension = ".tree";
  return file + output_extension;
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
  if (!is_file(alpha_file(file))) {
    std::cout << "File doesn't exists: " << alpha_file(file) << std::endl;
    return;
  }

  std::string source = read(alpha_file(file));
  compiler::Compiler compiler(source);

  check_result(alpha_file(file), token_file(file), source, compiler.tokens());
  check_result(token_file(file), tree_file(file), compiler.tokens(), compiler.tree());
}
