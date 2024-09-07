#include <lartc/external_errors.hh>
#include <iostream>

inline uint64_t find_next_newline(const char* source_code, uint64_t byte_start) {
  while(source_code[byte_start] != '\0' && source_code[byte_start] != '\n')
    byte_start++;
  return byte_start;
}

template<typename Point>
std::ostream& print_line_of_source_code_point(const char* source_code, Point& point, uint64_t byte_start) {
  uint64_t byte_of_first_character_of_line = byte_start - point.column;
  uint64_t byte_of_last_character_of_line = find_next_newline(source_code, byte_start);

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

void throw_type_is_not_dereferenceable_error(FileDB::Point& point, Declaration* context, Type* type) {
  FileDB::Point::Print(std::cerr, point);
  std::cerr << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": type is not dereferenceable '";
  Type::Print(std::cerr, type) << "'" << std::endl;
  std::cerr << " inside of declaration '";
  Declaration::PrintShort(std::cerr, context);
  std::cerr << "'" << std::endl;

  print_line_of_source_code_point(point.file->source_code, point, point.byte_start);
}

void throw_module_has_no_type_error(FileDB::Point& point, Declaration* context, Symbol& symbol) {
  FileDB::Point::Print(std::cerr, point);
  std::cerr << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": a module has not type '";
  Symbol::Print(std::cerr, symbol) << "'" << std::endl;
  std::cerr << " inside of declaration '";
  Declaration::PrintShort(std::cerr, context);
  std::cerr << "'" << std::endl;

  print_line_of_source_code_point(point.file->source_code, point, point.byte_start);
}

void throw_type_cannot_be_algebraically_manipulated_error(FileDB::Point& point, Declaration* context, Type* type) {
  FileDB::Point::Print(std::cerr, point);
  std::cerr << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": type cannot be algebraically manipulated '";
  Type::Print(std::cerr, type) << "'" << std::endl;
  std::cerr << " inside of declaration '";
  Declaration::PrintShort(std::cerr, context);
  std::cerr << "'" << std::endl;

  print_line_of_source_code_point(point.file->source_code, point, point.byte_start);
}

void throw_type_is_not_callable_error(FileDB::Point& point, Declaration* context, Type* non_callable_type) {
  FileDB::Point::Print(std::cerr, point);
  std::cerr << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": called type is not callable '";
  Type::Print(std::cerr, non_callable_type) << "'" << std::endl;
  std::cerr << " inside of declaration '";
  Declaration::PrintShort(std::cerr, context);
  std::cerr << "'" << std::endl;

  print_line_of_source_code_point(point.file->source_code, point, point.byte_start);
}

void throw_wrong_parameter_number_error(FileDB::Point& point, Declaration* context, Type* fn_type) {
  FileDB::Point::Print(std::cerr, point);
  std::cerr << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": wrong parameter number for function of type '";
  Type::Print(std::cerr, fn_type) << "'" << std::endl;
  std::cerr << " inside of declaration '";
  Declaration::PrintShort(std::cerr, context);
  std::cerr << "'" << std::endl;

  print_line_of_source_code_point(point.file->source_code, point, point.byte_start);
}

void throw_type_is_not_implicitly_castable_to(FileDB::Point& point, Declaration* context, Type* src_type, Type* dst_type) {
  FileDB::Point::Print(std::cerr, point);
  std::cerr << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": type '";
  Type::Print(std::cerr, src_type) << "'";
  std::cerr << "' is not implicitly castable to '";
  Type::Print(std::cerr, dst_type) << "' and must be casted explicitly" << std::endl;
  std::cerr << " inside of declaration '";
  Declaration::PrintShort(std::cerr, context);
  std::cerr << "'" << std::endl;

  print_line_of_source_code_point(point.file->source_code, point, point.byte_start);
}

void throw_right_operand_of_dot_operator_should_be_a_symbol(FileDB::Point& point, Declaration* context) {
  FileDB::Point::Print(std::cerr, point);
  std::cerr << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": right operand of dot operator should be a symbol" << std::endl;
  std::cerr << "inside of declaration '";
  Declaration::PrintShort(std::cerr, context);
  std::cerr << "'" << std::endl;

  print_line_of_source_code_point(point.file->source_code, point, point.byte_start);
}

void throw_left_operand_of_dot_operator_should_be_a_struct(FileDB::Point& point, Declaration* context, Type* non_struct_type) {
  FileDB::Point::Print(std::cerr, point);
  std::cerr << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": left operand of dot operator should be a struct, instead is '";
  Type::Print(std::cerr, non_struct_type) << "'" << std::endl;
  std::cerr << " inside of declaration '";
  Declaration::PrintShort(std::cerr, context);
  std::cerr << "'" << std::endl;

  print_line_of_source_code_point(point.file->source_code, point, point.byte_start);
}

void throw_struct_has_not_named_field(FileDB::Point& point, Declaration* context, Type* struct_type, Symbol& field_name) {
  FileDB::Point::Print(std::cerr, point);
  std::cerr << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": struct '";
  Type::Print(std::cerr, struct_type);
  Symbol::Print(std::cerr << "' has not named field '", field_name) << "'" << std::endl;
  std::cerr << " inside of declaration '";
  Declaration::PrintShort(std::cerr, context);
  std::cerr << "'" << std::endl;

  print_line_of_source_code_point(point.file->source_code, point, point.byte_start);
}

void throw_right_operand_of_arrow_operator_should_be_a_symbol(FileDB::Point& point, Declaration* context) {
  FileDB::Point::Print(std::cerr, point);
  std::cerr << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": right operand of arrow operator should be a symbol" << std::endl;
  std::cerr << "inside of declaration '";
  Declaration::PrintShort(std::cerr, context);
  std::cerr << "'" << std::endl;

  print_line_of_source_code_point(point.file->source_code, point, point.byte_start);
}

void throw_left_operand_of_arrow_operator_should_be_a_pointer(FileDB::Point& point, Declaration* context, Type* non_struct_type) {
  FileDB::Point::Print(std::cerr, point);
  std::cerr << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": left operand of arrow operator should be a pointer, instead is '";
  Type::Print(std::cerr, non_struct_type) << "'" << std::endl;
  std::cerr << " inside of declaration '";
  Declaration::PrintShort(std::cerr, context);
  std::cerr << "'" << std::endl;

  print_line_of_source_code_point(point.file->source_code, point, point.byte_start);
}

void throw_pointed_left_operand_of_arrow_operator_should_be_a_struct(FileDB::Point& point, Declaration* context, Type* non_struct_type) {
  FileDB::Point::Print(std::cerr, point);
  std::cerr << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": pointed left operand of arrow operator should be a struct, instead is '";
  Type::Print(std::cerr, non_struct_type) << "'" << std::endl;
  std::cerr << " inside of declaration '";
  Declaration::PrintShort(std::cerr, context);
  std::cerr << "'" << std::endl;

  print_line_of_source_code_point(point.file->source_code, point, point.byte_start);
}

void throw_types_cannot_be_algebraically_manipulated_error(FileDB::Point& point, Declaration* context, Type* left_type, Type* right_type) {
  FileDB::Point::Print(std::cerr, point);
  std::cerr << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": types  '";
  Type::Print(std::cerr, left_type) << "' and ";
  Type::Print(std::cerr, right_type) << "' cannot be algebraically manipulated together" << std::endl;
  std::cerr << " inside of declaration '";
  Declaration::PrintShort(std::cerr, context);
  std::cerr << "'" << std::endl;

  print_line_of_source_code_point(point.file->source_code, point, point.byte_start);
}

void throw_type_cannot_be_logically_manipulated_error(FileDB::Point& point, Declaration* context, Type* type) {
  FileDB::Point::Print(std::cerr, point);
  std::cerr << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": type cannot be logically manipulated '";
  Type::Print(std::cerr, type) << "'" << std::endl;
  std::cerr << " inside of declaration '";
  Declaration::PrintShort(std::cerr, context);
  std::cerr << "'" << std::endl;

  print_line_of_source_code_point(point.file->source_code, point, point.byte_start);
}

void throw_types_cannot_be_logically_manipulated_error(FileDB::Point& point, Declaration* context, Type* left_type, Type* right_type) {
  FileDB::Point::Print(std::cerr, point);
  std::cerr << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": types  '";
  Type::Print(std::cerr, left_type) << "' and ";
  Type::Print(std::cerr, right_type) << "' cannot be logically manipulated together" << std::endl;
  std::cerr << " inside of declaration '";
  Declaration::PrintShort(std::cerr, context);
  std::cerr << "'" << std::endl;

  print_line_of_source_code_point(point.file->source_code, point, point.byte_start);
}
