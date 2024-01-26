#include <string>
#include <iostream>

#include "test.h"
#include "readwrite.h"
#include "tokeniser.h"


void test_file(const std::string& file) {
  const std::string input_extension = ".alpha";
  const std::string test_extension = ".tokens.test";

  const std::string input_file = file + input_extension;
  const std::string test_file = file + test_extension;

  if (!is_file(file + input_extension)) {
    std::cout << "File " << input_file << " doesn't exists" << std::endl;
    return;
  }

  std::string content = read(input_file);
  std::vector<tokeniser::Token> tokens = tokeniser::tokenise(content);
  std::string result = to_string(tokens);

  bool write_to_file = true;
  if (is_file(test_file)) {
    std::string test_content = read(test_file);
    if (test_content == result) {
      std::cout << "File " << file << " passed" << std::endl;
      return;
    }
    std::cout << "File " << file << " failed" << std::endl;
    std::cout << "Input:" << std::endl;
    std::cout << content << std::endl;
    std::cout << "Output:" << std::endl;
    std::cout << result << std::endl;
    std::cout << "Correct:" << std::endl;
    std::cout << test_content << std::endl;
    write_to_file = user_approval("Do you want to override [Y/n]? ");
  }

  if (!write_to_file) return;

  write(test_file, result);
  std::cout << "File " << file << " saved" << std::endl;
}
