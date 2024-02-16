#include "source.h"

Source::Source(std::string source, Printer* printer) {
  lines = split(source);
  printer->print_lines(this);
}

Source::operator std::string() const {
  std::string result = "";
  for (const std::string& line: lines) {
    result += line + "\n";
  }
  return result;
}

int Source::size() const { return lines.size(); }

std::string Source::line(int index) const {
  return lines[index - 1];
}

std::string Source::line(int index, int from, int to) const {
  return lines[index - 1].substr(from, to - from);
}


std::vector<std::string> Source::split(std::string content) {
  std::vector<std::string> result;
  int last = 0;
  for (int index = 0; index < (int) content.size(); index++) {
    if (content[index] != '\n') continue;
    result.push_back(content.substr(last, index - last));
    last = index + 1;
  }
  return result;
}
