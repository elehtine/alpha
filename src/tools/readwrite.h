#ifndef READWRITE_H
#define READWRITE_H

#include <string>
#include <vector>
#include <memory>

#include "../tokeniser.h"

std::string to_string(const std::vector<tokeniser::Type>& vec);

template<typename T>
std::string to_string(const std::vector<std::unique_ptr<T>>& vec) {
  std::string result = "";
  for (const std::unique_ptr<T>& element: vec) {
    result += std::string(*element) + "\n";
  }
  return result;
}

template<typename T>
std::string to_string(std::vector<T> vec) {
  std::string result = "";
  for (const T& element: vec) {
    result += std::string(element) + "\n";
  }
  return result;
}

bool user_approval(std::string prompt);

bool is_file(const std::string& file);

std::string read(const std::string& file);

void write(const std::string& file, const std::string& content);

#endif
