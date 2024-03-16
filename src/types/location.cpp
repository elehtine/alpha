#include "location.h"

#include "../tools/exceptions.h"


Location::Location(int row, int column, std::string line):
  row(row), column(column), line(line)
{}

Location::Location(const Location& loc):
  row(loc.row), column(loc.column), line(loc.line)
{}

Location::operator std::string() const {
  return "(" + std::to_string(row) + "," + std::to_string(column) + ")";
}

std::string Location::error_mark() const {
  std::string result = "";
  result += line + "\n";
  result += std::string(column, ' ') + "^- ";
  return result;
}

std::string Location::error(std::string message) const {
  std::string result = "in line " + std::to_string(row) + "\n";
  result += message + "\n";
  result += line + "\n";
  result += std::string(column, ' ') + "^- here";
  return result;
}
