#include <lartc/external_errors.hh>
#include <iostream>

#define CERR std::cerr

inline std::uintmax_t find_next_newline(const char* source_code, std::uintmax_t byte_start) {
  while(source_code[byte_start] != '\0' && source_code[byte_start] != '\n')
    byte_start++;
  return byte_start;
}

template<typename Point>
std::ostream& print_line_of_source_code_point(const char* source_code, Point& point, std::uintmax_t byte_start) {
  std::uintmax_t byte_of_first_character_of_line = byte_start - point.column;
  std::uintmax_t byte_of_last_character_of_line = find_next_newline(source_code, byte_start);

  CERR << std::string(source_code + byte_of_first_character_of_line, byte_of_last_character_of_line - byte_of_first_character_of_line) << std::endl;
  return CERR << std::string(point.column, ' ') << "^" << std::endl;
}

// Cst Checking
void throw_syntax_error(const char* filepath, TSPoint& point, const char* node_symbol_name, const char* source_code, std::uintmax_t byte_start) {
  CERR << filepath << ":" << point.row+1 << ":" << point.column+1 << ": " << RED_TEXT << "syntax error" << NORMAL_TEXT << ": unexpected token inside of " << node_symbol_name << std::endl;
  print_line_of_source_code_point(source_code, point, byte_start);
}
void throw_parsed_integer_is_too_large(const char* filepath, TSPoint& point, const char* source_code, std::uintmax_t byte_start) {
  CERR << filepath << ":" << point.row+1 << ":" << point.column+1 << ": " << RED_TEXT << "parsing error" << NORMAL_TEXT << ": integer is too large " << std::endl;
  print_line_of_source_code_point(source_code, point, byte_start);
}

// Duplicate Declaration/Definitions
void throw_duplicate_declaration_matches_name_but_not_kind(FileDB& file_db, FileDB::Point& older_point, FileDB::Point& latest_point) {
  FileDB::Point::Print(CERR, file_db, latest_point);
  CERR << ": " << RED_TEXT << "duplicate declaration error" << NORMAL_TEXT << ": matches name but not kind" << std::endl;
  print_line_of_source_code_point(file_db.files[latest_point.file].source_code, latest_point, latest_point.byte_start);

  FileDB::Point::Print(CERR, file_db, older_point);
  CERR << ": " << AZURE_TEXT << "reference" << NORMAL_TEXT << ": already declared here" << std::endl;
  print_line_of_source_code_point(file_db.files[older_point.file].source_code, older_point, older_point.byte_start);
}

void throw_duplicate_type_definition_doesnt_match(FileDB& file_db, FileDB::Point& older_point, FileDB::Point& latest_point) {
  FileDB::Point::Print(CERR, file_db, latest_point);
  CERR << ": " << RED_TEXT << "duplicate definition error" << NORMAL_TEXT << ": defined type doesn't match" << std::endl;
  print_line_of_source_code_point(file_db.files[latest_point.file].source_code, latest_point, latest_point.byte_start);

  FileDB::Point::Print(CERR, file_db, older_point);
  CERR << ": " << AZURE_TEXT << "reference" << NORMAL_TEXT << ": already defined here" << std::endl;
  print_line_of_source_code_point(file_db.files[older_point.file].source_code, older_point, older_point.byte_start);
}

void throw_duplicate_function_definition(FileDB& file_db, FileDB::Point& older_point, FileDB::Point& latest_point) {
  FileDB::Point::Print(CERR, file_db, latest_point);
  CERR << ": " << RED_TEXT << "duplicate definition error" << NORMAL_TEXT << ": duplicate function definition" << std::endl;
  print_line_of_source_code_point(file_db.files[latest_point.file].source_code, latest_point, latest_point.byte_start);

  FileDB::Point::Print(CERR, file_db, older_point);
  CERR << ": " << AZURE_TEXT << "reference" << NORMAL_TEXT << ": already defined here" << std::endl;
  print_line_of_source_code_point(file_db.files[older_point.file].source_code, older_point, older_point.byte_start);
}

void throw_duplicate_function_declaration_return_type_doesnt_match(FileDB& file_db, FileDB::Point& older_point, FileDB::Point& latest_point) {
  FileDB::Point::Print(CERR, file_db, latest_point);
  CERR << ": " << RED_TEXT << "duplicate declaration error" << NORMAL_TEXT << ": return type doesn't match previous declaration" << std::endl;
  print_line_of_source_code_point(file_db.files[latest_point.file].source_code, latest_point, latest_point.byte_start);

  FileDB::Point::Print(CERR, file_db, older_point);
  CERR << ": " << AZURE_TEXT << "reference" << NORMAL_TEXT << ": already declared here" << std::endl;
  print_line_of_source_code_point(file_db.files[older_point.file].source_code, older_point, older_point.byte_start);
}

void throw_duplicate_function_declaration_parameter_types_dont_match(FileDB& file_db, FileDB::Point& older_point, FileDB::Point& latest_point) {
  FileDB::Point::Print(CERR, file_db, latest_point);
  CERR << ": " << RED_TEXT << "duplicate declaration error" << NORMAL_TEXT << ": parameter types don't match previous declaration" << std::endl;
  print_line_of_source_code_point(file_db.files[latest_point.file].source_code, latest_point, latest_point.byte_start);

  FileDB::Point::Print(CERR, file_db, older_point);
  CERR << ": " << AZURE_TEXT << "reference" << NORMAL_TEXT << ": already declared here" << std::endl;
  print_line_of_source_code_point(file_db.files[older_point.file].source_code, older_point, older_point.byte_start);
}

void throw_duplicate_function_declaration_wrong_parameter_number(FileDB& file_db, FileDB::Point& older_point, FileDB::Point& latest_point) {
  FileDB::Point::Print(CERR, file_db, latest_point);
  CERR << ": " << RED_TEXT << "duplicate declaration error" << NORMAL_TEXT << ": parameter number doesn't match previous declaration" << std::endl;
  print_line_of_source_code_point(file_db.files[latest_point.file].source_code, latest_point, latest_point.byte_start);

  FileDB::Point::Print(CERR, file_db, older_point);
  CERR << ": " << AZURE_TEXT << "reference" << NORMAL_TEXT << ": already declared here" << std::endl;
  print_line_of_source_code_point(file_db.files[older_point.file].source_code, older_point, older_point.byte_start);
}

// Include Machanism
void throw_unable_to_resolve_include_filepath(FileDB& file_db, const FileDB::Point& point, const std::string& filepath) {
  FileDB::Point::Print(CERR, file_db, point);
  CERR << ": " << RED_TEXT << "include error" << NORMAL_TEXT << ": unable to find " << filepath << std::endl;
  print_line_of_source_code_point(file_db.files[point.file].source_code, point, point.byte_start);
}

// Name Resolution
void throw_name_resolution_error(FileDB& file_db, FileDB::Point& point, Declaration* context, Symbol& symbol) {
  FileDB::Point::Print(CERR, file_db, point);
  CERR << ": " << RED_TEXT << "syntax error" << NORMAL_TEXT << ": unable to resolve symbol '";
  Symbol::Print(CERR, symbol) << "'" << std::endl;
  CERR << " inside of declaration '";
  Declaration::PrintShort(CERR, context);
  CERR << "'" << std::endl;

  print_line_of_source_code_point(file_db.files[point.file].source_code, point, point.byte_start);
}

// Decl Type Checking
void throw_a_type_definition_cannot_reference_a_non_type_declaration(FileDB& file_db, FileDB::Point& point, Declaration* type_decl, Declaration* non_type_decl) {
  FileDB::Point::Print(CERR, file_db, point);
  CERR << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": cannot reference non-type declaration '";
  Declaration::PrintShort(CERR, non_type_decl) << "'" << std::endl;
  CERR << " inside type declaration '";
  Declaration::PrintShort(CERR, type_decl);
  CERR << "'" << std::endl;

  print_line_of_source_code_point(file_db.files[point.file].source_code, point, point.byte_start);
}

void throw_cyclic_dependency_between_types_is_not_protected_by_usage_of_pointers(FileDB& file_db, FileDB::Point& point, Declaration* type_decl, Declaration* requested_type_decl) {
  FileDB::Point::Print(CERR, file_db, point);
  CERR << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": cyclic dependency with type '";
  Declaration::PrintShort(CERR, requested_type_decl) << "'" << std::endl;
  CERR << " requiring '";
  Declaration::PrintShort(CERR, type_decl);
  CERR << "' is not protected by usage of pointers" << std::endl;

  print_line_of_source_code_point(file_db.files[point.file].source_code, point, point.byte_start);
}

// Type Checking
void throw_type_is_not_dereferenceable_error(FileDB& file_db, FileDB::Point& point, Declaration* context, Type* type) {
  FileDB::Point::Print(CERR, file_db, point);
  CERR << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": type is not dereferenceable '";
  Type::Print(CERR, type) << "'" << std::endl;
  CERR << " inside of declaration '";
  Declaration::PrintShort(CERR, context);
  CERR << "'" << std::endl;

  print_line_of_source_code_point(file_db.files[point.file].source_code, point, point.byte_start);
}

void throw_module_has_no_type_error(FileDB& file_db, FileDB::Point& point, Declaration* context, Symbol& symbol) {
  FileDB::Point::Print(CERR, file_db, point);
  CERR << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": a module has not type '";
  Symbol::Print(CERR, symbol) << "'" << std::endl;
  CERR << " inside of declaration '";
  Declaration::PrintShort(CERR, context);
  CERR << "'" << std::endl;

  print_line_of_source_code_point(file_db.files[point.file].source_code, point, point.byte_start);
}

void throw_type_cannot_be_algebraically_manipulated_error(FileDB& file_db, FileDB::Point& point, Declaration* context, Type* type) {
  FileDB::Point::Print(CERR, file_db, point);
  CERR << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": type cannot be algebraically manipulated '";
  Type::Print(CERR, type) << "'" << std::endl;
  CERR << " inside of declaration '";
  Declaration::PrintShort(CERR, context);
  CERR << "'" << std::endl;

  print_line_of_source_code_point(file_db.files[point.file].source_code, point, point.byte_start);
}

void throw_type_is_not_callable_error(FileDB& file_db, FileDB::Point& point, Declaration* context, Type* non_callable_type) {
  FileDB::Point::Print(CERR, file_db, point);
  CERR << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": called type '";
  Type::Print(CERR, non_callable_type) << "' is not callable" << std::endl;
  CERR << " inside of declaration '";
  Declaration::PrintShort(CERR, context);
  CERR << "'" << std::endl;

  print_line_of_source_code_point(file_db.files[point.file].source_code, point, point.byte_start);
}

void throw_wrong_parameter_number_error(FileDB& file_db, FileDB::Point& point, Declaration* context, Type* fn_type) {
  FileDB::Point::Print(CERR, file_db, point);
  CERR << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": wrong parameter number for function of type '";
  Type::Print(CERR, fn_type) << "'" << std::endl;
  CERR << " inside of declaration '";
  Declaration::PrintShort(CERR, context);
  CERR << "'" << std::endl;

  print_line_of_source_code_point(file_db.files[point.file].source_code, point, point.byte_start);
}

void throw_type_is_not_implicitly_castable_to(FileDB& file_db, FileDB::Point& point, Declaration* context, Type* src_type, Type* dst_type) {
  FileDB::Point::Print(CERR, file_db, point);
  CERR << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": type '";
  Type::Print(CERR, src_type) << "'";
  CERR << " is not implicitly castable to '";
  Type::Print(CERR, dst_type) << "' and must be casted explicitly" << std::endl;
  CERR << " inside of declaration '";
  Declaration::PrintShort(CERR, context);
  CERR << "'" << std::endl;

  print_line_of_source_code_point(file_db.files[point.file].source_code, point, point.byte_start);
}

void throw_return_type_is_not_implicitly_castable_to(FileDB& file_db, FileDB::Point& point, Declaration* context, Type* src_type, Type* dst_type) {
  FileDB::Point::Print(CERR, file_db, point);
  CERR << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": statement returns a value of type '";
  Type::Print(CERR, src_type) << "'";
  CERR << " when the declared function type required '";
  Type::Print(CERR, dst_type) << "' as return type. It may need to be casted explicitly" << std::endl;
  CERR << " inside of declaration '";
  Declaration::PrintShort(CERR, context);
  CERR << "'" << std::endl;

  print_line_of_source_code_point(file_db.files[point.file].source_code, point, point.byte_start);
}

void throw_right_operand_of_dot_operator_should_be_a_symbol(FileDB& file_db, FileDB::Point& point, Declaration* context) {
  FileDB::Point::Print(CERR, file_db, point);
  CERR << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": right operand of dot operator should be a symbol" << std::endl;
  CERR << "inside of declaration '";
  Declaration::PrintShort(CERR, context);
  CERR << "'" << std::endl;

  print_line_of_source_code_point(file_db.files[point.file].source_code, point, point.byte_start);
}

void throw_left_operand_of_dot_operator_should_be_a_struct(FileDB& file_db, FileDB::Point& point, Declaration* context, Type* non_struct_type) {
  FileDB::Point::Print(CERR, file_db, point);
  CERR << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": left operand of dot operator should be a struct, instead is '";
  Type::Print(CERR, non_struct_type) << "'" << std::endl;
  CERR << " inside of declaration '";
  Declaration::PrintShort(CERR, context);
  CERR << "'" << std::endl;

  print_line_of_source_code_point(file_db.files[point.file].source_code, point, point.byte_start);
}

void throw_struct_has_not_named_field(FileDB& file_db, FileDB::Point& point, Declaration* context, Type* struct_type, Symbol& field_name) {
  FileDB::Point::Print(CERR, file_db, point);
  CERR << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": struct '";
  Type::Print(CERR, struct_type);
  Symbol::Print(CERR << "' has not named field '", field_name) << "'" << std::endl;
  CERR << " inside of declaration '";
  Declaration::PrintShort(CERR, context);
  CERR << "'" << std::endl;

  print_line_of_source_code_point(file_db.files[point.file].source_code, point, point.byte_start);
}

void throw_right_operand_of_arrow_operator_should_be_a_symbol(FileDB& file_db, FileDB::Point& point, Declaration* context) {
  FileDB::Point::Print(CERR, file_db, point);
  CERR << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": right operand of arrow operator should be a symbol" << std::endl;
  CERR << "inside of declaration '";
  Declaration::PrintShort(CERR, context);
  CERR << "'" << std::endl;

  print_line_of_source_code_point(file_db.files[point.file].source_code, point, point.byte_start);
}

void throw_left_operand_of_array_access_should_be_a_pointer_or_an_array(FileDB& file_db, FileDB::Point& point, Declaration* context, Type* invalid_type) {
  FileDB::Point::Print(CERR, file_db, point);
  CERR << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": left operand of an array access should be an array or a pointer";
  Type::Print(CERR << ", found instead '", invalid_type) << "'" << std::endl;
  CERR << "inside of declaration '";
  Declaration::PrintShort(CERR, context);
  CERR << "'" << std::endl;

  print_line_of_source_code_point(file_db.files[point.file].source_code, point, point.byte_start);
}

void throw_right_operand_of_array_access_should_be_an_integer(FileDB& file_db, FileDB::Point& point, Declaration* context, Type* invalid_type) {
  FileDB::Point::Print(CERR, file_db, point);
  CERR << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": right operand of an array access should be an integer";
  Type::Print(CERR << ", found instead '", invalid_type) << "'" << std::endl;
  CERR << "inside of declaration '";
  Declaration::PrintShort(CERR, context);
  CERR << "'" << std::endl;

  print_line_of_source_code_point(file_db.files[point.file].source_code, point, point.byte_start);
}

void throw_left_operand_of_arrow_operator_should_be_a_pointer(FileDB& file_db, FileDB::Point& point, Declaration* context, Type* non_struct_type) {
  FileDB::Point::Print(CERR, file_db, point);
  CERR << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": left operand of arrow operator should be a pointer, instead is '";
  Type::Print(CERR, non_struct_type) << "'" << std::endl;
  CERR << " inside of declaration '";
  Declaration::PrintShort(CERR, context);
  CERR << "'" << std::endl;

  print_line_of_source_code_point(file_db.files[point.file].source_code, point, point.byte_start);
}

void throw_pointed_left_operand_of_arrow_operator_should_be_a_struct(FileDB& file_db, FileDB::Point& point, Declaration* context, Type* non_struct_type) {
  FileDB::Point::Print(CERR, file_db, point);
  CERR << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": pointed left operand of arrow operator should be a struct, instead is '";
  Type::Print(CERR, non_struct_type) << "'" << std::endl;
  CERR << " inside of declaration '";
  Declaration::PrintShort(CERR, context);
  CERR << "'" << std::endl;

  print_line_of_source_code_point(file_db.files[point.file].source_code, point, point.byte_start);
}

void throw_types_cannot_be_algebraically_manipulated_error(FileDB& file_db, FileDB::Point& point, Declaration* context, Type* left_type, Type* right_type) {
  FileDB::Point::Print(CERR, file_db, point);
  CERR << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": types  '";
  Type::Print(CERR, left_type) << "' and '";
  Type::Print(CERR, right_type) << "' cannot be algebraically manipulated together" << std::endl;
  CERR << " inside of declaration '";
  Declaration::PrintShort(CERR, context);
  CERR << "'" << std::endl;

  print_line_of_source_code_point(file_db.files[point.file].source_code, point, point.byte_start);
}

void throw_type_cannot_be_logically_manipulated_error(FileDB& file_db, FileDB::Point& point, Declaration* context, Type* type) {
  FileDB::Point::Print(CERR, file_db, point);
  CERR << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": type cannot be logically manipulated '";
  Type::Print(CERR, type) << "'" << std::endl;
  CERR << " inside of declaration '";
  Declaration::PrintShort(CERR, context);
  CERR << "'" << std::endl;

  print_line_of_source_code_point(file_db.files[point.file].source_code, point, point.byte_start);
}

void throw_types_cannot_be_logically_manipulated_error(FileDB& file_db, FileDB::Point& point, Declaration* context, Type* left_type, Type* right_type) {
  FileDB::Point::Print(CERR, file_db, point);
  CERR << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << ": types  '";
  Type::Print(CERR, left_type) << "' and '";
  Type::Print(CERR, right_type) << "' cannot be logically manipulated together" << std::endl;
  CERR << " inside of declaration '";
  Declaration::PrintShort(CERR, context);
  CERR << "'" << std::endl;

  print_line_of_source_code_point(file_db.files[point.file].source_code, point, point.byte_start);
}

void throw_uncaught_type_checker_error(FileDB& file_db, FileDB::Point& point, Declaration* context, Expression* expr) {
  FileDB::Point::Print(CERR, file_db, point);
  CERR << ": " << RED_TEXT << "type checking error" << NORMAL_TEXT << " with expression '";
  Expression::Print(CERR, expr) << "' inside of declaration '";
  Declaration::PrintShort(CERR, context);
  CERR << "'" << std::endl;
  print_line_of_source_code_point(file_db.files[point.file].source_code, point, point.byte_start);
}
