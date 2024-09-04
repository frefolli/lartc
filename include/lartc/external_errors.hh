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

inline uint64_t find_next_newline(const char* source_code, uint64_t byte_start) {
  while(source_code[byte_start] != '0' && source_code[byte_start] != '\n')
    byte_start++;
  return byte_start;
}

void throw_syntax_error(const char* filepath, TSPoint& point, const char* node_symbol_name, const char* source_code, uint64_t byte_start);
void throw_name_resolution_error(FileDB::Point& point, Declaration* context, Symbol& symbol);
#endif//LARTC_EXTERNAL_ERROR
