#include <iostream>
#include <string>

#include "tokenizer.h"
#include "readwrite.h"

void tokenize(const std::string& file) {
  std::cout << "tokenize: " << file << std::endl;
  std::cout << "content:" << std::endl;
  std::cout << read(file) << std::endl;
}
