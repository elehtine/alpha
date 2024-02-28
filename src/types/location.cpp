#include "location.h"

#include "../tools/exceptions.h"


Location::Location(int row, int column, std::string line):
  row(row), column(column), line(line) {}

Location::operator std::string() const {
  return "(" + std::to_string(row) + "," + std::to_string(column) + ")";
}

TokeniseException Location::error() {
  std::string message = "'" + line.substr(column, 10) + "' in line ";
  message += std::to_string(row) + "\n";
  message += error_mark() + "unknown token";
  return message;
}

std::string Location::error_mark() {
  std::string result = "";
  result += line + "\n";
  result += std::string(column, ' ') + "^- ";
  return result;
}


