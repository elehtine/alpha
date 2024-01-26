#include <string>
#include <iostream>

#include "test.h"
#include "tools/readwrite.h"
#include "tokeniser.h"

std::string input_file(const std::string& file) {
  const std::string input_extension = ".alpha";
  return file + input_extension;
}

std::string output_file(const std::string& file) {
  const std::string output_extension = ".tokens";
  return file + output_extension;
}

void check_result(const std::string& file, const std::string& input, const std::string& output) {
  bool write_to_file = true;
  if (is_file(output_file(file))) {
    std::string correct = read(output_file(file));
    if (correct == output) {
      std::cout << "File passed: " << file << std::endl;
      return;
    }

    std::cout << "File failed: " << file << std::endl;
    std::cout << "Input:" << std::endl;
    std::cout << input << std::endl;
    std::cout << "Output:" << std::endl;
    std::cout << output << std::endl;
    std::cout << "Correct:" << std::endl;
    std::cout << correct << std::endl;
    write_to_file = user_approval("Do you want to override [Y/n]? ");
  }

  if (!write_to_file) return;

  write(output_file(file), output);
  std::cout << "File saved: " << file << std::endl;
}

void test_file(const std::string& file) {
  if (!is_file(input_file(file))) {
    std::cout << "File doesn't exists: " << input_file(file) << std::endl;
    return;
  }

  std::string input = read(input_file(file));
  std::vector<tokeniser::Token> tokens = tokeniser::tokenise(input);
  std::string output = to_string(tokens);

  check_result(file, input, output);
}
