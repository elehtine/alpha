#include <iostream>
#include <string>

#include "tokenizer.h"

std::string content(const std::string& file) {
  return "file content foo!";
}

void tokenize(std::string file) {
  std::cout << "tokenize: " << file << std::endl;
  std::cout << "content:" << std::endl;
  std::cout << content(file) << std::endl;
}
