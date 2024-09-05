#ifndef LARTC_EXTERNAL_ERROR
#define LARTC_EXTERNAL_ERROR
/* External errors are non-fatal errors that occur when user-feeded input is wrong.
 * Typically, this implies:
 * - Syntax Errors
 * - Duplicate Definition Errors
 * - Name Resolution Errors (globals, locals, parameters)
 * - Type Checking Errors (incompatible types, wrong arguments, bad field accesses)
 * */
#include <cstdint>
#include <lartc/terminal.hh>
#include <tree_sitter/api.h>
#include <lartc/ast/declaration.hh>
#include <lartc/ast/file_db.hh>
#include <lartc/ast/symbol.hh>

void throw_syntax_error(const char* filepath, TSPoint& point, const char* node_symbol_name, const char* source_code, uint64_t byte_start);
void throw_name_resolution_error(FileDB::Point& point, Declaration* context, Symbol& symbol);
void throw_type_is_not_dereferenceable_error(FileDB::Point& point, Declaration* context, Type* type);
void throw_module_has_no_type_error(FileDB::Point& point, Declaration* context, Symbol& symbol);
void throw_type_cannot_be_algebraically_manipulated_error(FileDB::Point& point, Declaration* context, Type* type);
void throw_type_is_not_callable_error(FileDB::Point& point, Declaration* context, Type* non_callable_type);
void throw_wrong_parameter_number_error(FileDB::Point& point, Declaration* context, Type* fn_type);
#endif//LARTC_EXTERNAL_ERROR
