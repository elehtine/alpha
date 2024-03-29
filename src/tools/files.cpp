#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <boost/filesystem.hpp>

#include "../types/token.h"
#include "../types/source.h"

#include "files.h"


std::vector<std::string> test_files() {
  const boost::filesystem::path dir { "tests" };
  std::vector<std::string> files;
  for (auto const& file: boost::filesystem::directory_iterator(dir)) {
    files.push_back(file.path().string());
  }
  return files;
}

bool user_approval(std::string prompt) {
  std::cout << prompt;

  std::string approval;
  std::cin >> approval;
  std::vector<std::string> positive = { "Y", "y", "yes" };
  for (const std::string& possible: positive) {
    if (approval == possible) return true;
  }
  return false;
}

bool is_file(const std::string& file) {
  return std::ifstream(file, std::ios::in).good();
}

std::string read(const std::string& file) {
  std::string text;
  std::string s;
  std::ifstream in(file, std::ios::in);
  while (getline(in, s)) {
    text.append(s);
    text.append("\n");
  }
  return text;
}

void write(const std::string& file, const std::string& content) {
  std::ofstream out(file);
  out << content;
  out.close();
}
