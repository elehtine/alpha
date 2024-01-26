#include <fstream>
#include <string>


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
