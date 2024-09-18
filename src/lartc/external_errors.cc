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

// Cst Checking
void throw_syntax_error(const char* filepath, TSPoint& point, const char* node_symbol_name, const char* source_code, uint64_t byte_start) {
  std::cerr << filepath << ":" << point.row+1 << ":" << point.column+1 << ": " << RED_TEXT << "syntax error" << NORMAL_TEXT << ": unexpected token inside of " << node_symbol_name << std::endl;
  print_line_of_source_code_point(source_code, point, byte_start);
}

// Duplicate Declaration/Definitions
void throw_duplicate_declaration_matches_name_but_not_kind(FileDB::Point& older_point, FileDB::Point& latest_point) {
  FileDB::Point::Print(std::cerr, latest_point);
  std::cerr << ": " << RED_TEXT << "duplicate declaration error" << NORMAL_TEXT << ": matches name but not kind" << std::endl;
  print_line_of_source_code_point(latest_point.file->source_code, latest_point, latest_point.byte_start);

  FileDB::Point::Print(std::cerr, older_point);
  std::cerr << ": " << AZURE_TEXT << "reference" << NORMAL_TEXT << ": already declared here" << std::endl;
  print_line_of_source_code_point(older_point.file->source_code, older_point, older_point.byte_start);
}

void throw_duplicate_type_definition_doesnt_match(FileDB::Point& older_point, FileDB::Point& latest_point) {
  FileDB::Point::Print(std::cerr, latest_point);
  std::cerr << ": " << RED_TEXT << "duplicate definition error" << NORMAL_TEXT << ": defined type doesn't match" << std::endl;
  print_line_of_source_code_point(latest_point.file->source_code, latest_point, latest_point.byte_start);

  FileDB::Point::Print(std::cerr, older_point);
  std::cerr << ": " << AZURE_TEXT << "reference" << NORMAL_TEXT << ": already defined here" << std::endl;
  print_line_of_source_code_point(older_point.file->source_code, older_point, older_point.byte_start);
}

void throw_duplicate_function_definition(FileDB::Point& older_point, FileDB::Point& latest_point) {
  FileDB::Point::Print(std::cerr, latest_point);
  std::cerr << ": " << RED_TEXT << "duplicate definition error" << NORMAL_TEXT << ": duplicate function definition" << std::endl;
  print_line_of_source_code_point(latest_point.file->source_code, latest_point, latest_point.byte_start);

  FileDB::Point::Print(std::cerr, older_point);
  std::cerr << ": " << AZURE_TEXT << "reference" << NORMAL_TEXT << ": already defined here" << std::endl;
  print_line_of_source_code_point(older_point.file->source_code, older_point, older_point.byte_start);
}

void throw_duplicate_function_declaration_return_type_doesnt_match(FileDB::Point& older_point, FileDB::Point& latest_point) {
  FileDB::Point::Print(std::cerr, latest_point);
  std::cerr << ": " << RED_TEXT << "duplicate declaration error" << NORMAL_TEXT << ": return type doesn't match previous declaration" << std::endl;
  print_line_of_source_code_point(latest_point.file->source_code, latest_point, latest_point.byte_start);

  FileDB::Point::Print(std::cerr, older_point);
  std::cerr << ": " << AZURE_TEXT << "reference" << NORMAL_TEXT << ": already declared here" << std::endl;
  print_line_of_source_code_point(older_point.file->source_code, older_point, older_point.byte_start);
}

void throw_duplicate_function_declaration_parameter_types_dont_match(FileDB::Point& older_point, FileDB::Point& latest_point) {
  FileDB::Point::Print(std::cerr, latest_point);
  std::cerr << ": " << RED_TEXT << "duplicate declaration error" << NORMAL_TEXT << ": parameter types don't match previous declaration" << std::endl;
  print_line_of_source_code_point(latest_point.file->source_code, latest_point, latest_point.byte_start);

  FileDB::Point::Print(std::cerr, older_point);
  std::cerr << ": " << AZURE_TEXT << "reference" << NORMAL_TEXT << ": already declared here" << std::endl;
  print_line_of_source_code_point(older_point.file->source_code, older_point, older_point.byte_start);
}

void throw_duplicate_function_declaration_wrong_parameter_number(FileDB::Point& older_point, FileDB::Point& latest_point) {
  FileDB::Point::Print(std::cerr, latest_point);
  std::cerr << ": " << RED_TEXT << "duplicate declaration error" << NORMAL_TEXT << ": parameter number doesn't match previous declaration" << std::endl;
  print_line_of_source_code_point(latest_point.file->source_code, latest_point, latest_point.byte_start);

  FileDB::Point::Print(std::cerr, older_point);
  std::cerr << ": " << AZURE_TEXT << "reference" << NORMAL_TEXT << ": already declared here" << std::endl;
  print_line_of_source_code_point(older_point.file->source_code, older_point, older_point.byte_start);
}

// Name Resolution
void throw_name_resolution_error(FileDB::Point& point, Declaration* context, Symbol& symbol) {
  FileDB::Point::Print(std::cerr, point);
  std::cerr << ": " << RED_TEXT << "syntax error" << NORMAL_TEXT << ": unable to resolve symbol '";
  Symbol::Print(std::cerr, symbol) << "'" << std::endl;
  std::cerr << " inside of declaration '";
  Declaration::PrintShort(std::cerr, context);
  std::cerr << "'" << std::endl;

  print_line_of_source_code_point(point.file->source_code, point, point.byte_start);
}

// Decl Type Checking
void throw_a_type_definition_cannot_reference_a_non_type_declaration(FileDB::Point& point, Declaration* type_decl, Declaration* non_type_decl) {
  FileDB::Point::Print(std::cerr, point);
  std::cerr << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": cannot reference non-type declaration '";
  Declaration::PrintShort(std::cerr, non_type_decl) << "'" << std::endl;
  std::cerr << " inside type declaration '";
  Declaration::PrintShort(std::cerr, type_decl);
  std::cerr << "'" << std::endl;

  print_line_of_source_code_point(point.file->source_code, point, point.byte_start);
}

void throw_cyclic_dependency_between_types_is_not_protected_by_usage_of_pointers(FileDB::Point& point, Declaration* type_decl, Declaration* requested_type_decl) {
  FileDB::Point::Print(std::cerr, point);
  std::cerr << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": cyclic dependency with type '";
  Declaration::PrintShort(std::cerr, requested_type_decl) << "'" << std::endl;
  std::cerr << " requiring '";
  Declaration::PrintShort(std::cerr, type_decl);
  std::cerr << "' is not protected by usage of pointers" << std::endl;

  print_line_of_source_code_point(point.file->source_code, point, point.byte_start);
}

// Type Checking
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
  std::cerr << " is not implicitly castable to '";
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
  Type::Print(std::cerr, left_type) << "' and '";
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
  Type::Print(std::cerr, left_type) << "' and '";
  Type::Print(std::cerr, right_type) << "' cannot be logically manipulated together" << std::endl;
  std::cerr << " inside of declaration '";
  Declaration::PrintShort(std::cerr, context);
  std::cerr << "'" << std::endl;

  print_line_of_source_code_point(point.file->source_code, point, point.byte_start);
}
