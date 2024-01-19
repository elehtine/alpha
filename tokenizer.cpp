#include <iostream>
#include <fstream>
#include <string>

#include "tokenizer.h"

std::string read(const std::string& file) {
  std::ifstream in;
  std::string text;
  std::string s;
  in.open(file, std::ios::in);
  while (getline(in, s)) {
    text.append(s);
    text.append("\n");
  }
  return text;
}

void tokenize(const std::string& file) {
  std::cout << "tokenize: " << file << std::endl;
  std::cout << "content:" << std::endl;
  std::cout << read(file) << std::endl;
}
