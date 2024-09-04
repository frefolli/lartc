#include <lartc/external_errors.hh>
#include <iostream>

template<typename Point>
std::ostream& print_line_of_source_code_point(const char* source_code, Point& point, uint64_t byte_start) {
  uint64_t byte_of_first_character_of_line = byte_start - point.column;
  uint64_t byte_of_last_character_of_line = find_next_newline(source_code, byte_start);

  // WARN: this could be faster by:
  // - replacing the newline char with '\0'
  // - letting normal cstring print
  // - replacing the '\0' with newline char
  // The only problem is that this is a const char*, not a char*
  std::cerr << std::string(source_code + byte_of_first_character_of_line, byte_of_last_character_of_line - byte_of_first_character_of_line) << std::endl;
  return std::cerr << std::string(point.column, ' ') << "^" << std::endl;
}

void throw_syntax_error(const char* filepath, TSPoint& point, const char* node_symbol_name, const char* source_code, uint64_t byte_start) {
  std::cerr << filepath << ":" << point.row+1 << ":" << point.column+1 << ": " << RED_TEXT << "syntax error" << NORMAL_TEXT << ": unexpected token inside of " << node_symbol_name << std::endl;
  print_line_of_source_code_point(source_code, point, byte_start);
}

void throw_name_resolution_error(FileDB::Point& point, Declaration* context, Symbol& symbol) {
  FileDB::Point::Print(std::cerr, point);
  std::cerr << ": " << RED_TEXT << "syntax error" << NORMAL_TEXT << ": unable to resolve symbol '";
  Symbol::Print(std::cerr, symbol) << "'" << std::endl;
  std::cerr << " inside of declaration '";
  Declaration::PrintShort(std::cerr, context);
  std::cerr << "'" << std::endl;

  print_line_of_source_code_point(point.file->source_code, point, point.byte_start);
}
