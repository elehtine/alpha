#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <boost/filesystem.hpp>

#include "../types/token.h"
#include "readwrite.h"


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

template<typename T>
std::string to_string(std::vector<T> vec) {
  std::string result = "";
  for (const T& element: vec) {
    result += std::string(element) + "\n";
  }
  return result;
}

void UserPrinter::print_source(std::string source) {
  std::cout << "Source:" << std::endl;
  std::cout << source << std::endl;
}

void UserPrinter::print_tokens(std::vector<token::Token> tokens) {
  std::cout << "Tokens:" << std::endl;
  std::cout << to_string(tokens) << std::endl;
}

void UserPrinter::print_tree(ast::Expression* root) {
  std::cout << "AST:" << std::endl;
  std::cout << std::string(*root) << std::endl;
}

void UserPrinter::print_value(interpretation::Interpretation* interpretation) {
  std::cout << "Interpretation:" << std::endl;
  std::cout << std::string(*interpretation) << std::endl;
}

FilePrinter::FilePrinter(std::string name): name(name) {}

void FilePrinter::print_source(std::string source) {
  input = source;
}

void FilePrinter::print_tokens(std::vector<token::Token> tokens) {
  print(to_string(tokens), filename(FileType::tokens));
}

void FilePrinter::print_tree(ast::Expression* root) {
  if (root != nullptr) print(std::string(*root), filename(FileType::tree));
}

void FilePrinter::print_value(interpretation::Interpretation* interpretation) {
  print(std::string(*interpretation), filename(FileType::interpret));
}

std::string FilePrinter::filename(const FileType type) {
  std::string suffix = ".unknown";
  if (type == FileType::source) suffix = ".alpha";
  if (type == FileType::tokens) suffix = ".tokens";
  if (type == FileType::tree) suffix = ".tree";
  if (type == FileType::interpret) suffix = ".interpret";
  return name + suffix;
}

void FilePrinter::print(const std::string& result, const std::string& file) {
  std::string before = read(file);
  if (accept(result, before)) {
    std::cout << "File passed: " << file << std::endl;
    write(file, result);
  } else {
    std::cout << "File failed: " << file << std::endl;
  }
  input = result;
}

bool FilePrinter::accept(const std::string& result, const std::string& before) {
  if (result == before) {
    return true;
  }

  std::cout << "File failed! " << std::endl;
  std::cout << "Input:" << std::endl;
  std::cout << input << std::endl;
  std::cout << "Result:" << std::endl;
  std::cout << result << std::endl;
  std::cout << "Before:" << std::endl;
  std::cout << before << std::endl;
  return user_approval("Do you want to override [Y/n]? ");
}

