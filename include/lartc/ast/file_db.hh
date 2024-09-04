#ifndef LARTC_AST_FILE_DB
#define LARTC_AST_FILE_DB
#include <cstdint>
#include <map>
#include <lartc/ast/symbol.hh>
#include <tree_sitter/api.h>

struct FileDB {
  struct File {
    char* filepath;
    char* source_code;

    static std::ostream& Print(std::ostream& out, const File& file);
  };
  struct Point {
    File* file;
    uint64_t row;
    uint64_t column;
    uint64_t byte_start;
    uint64_t byte_end;

    static std::ostream& Print(std::ostream& out, const Point& point);
  };
  
  std::map<Symbol*, Point> symbol_points;
  std::vector<File> files;

  File* add_file(const char* filepath);
  void add_symbol(Symbol* symbol, TSNode& node);
  inline File* current_file() {
    return &files.back();
  }

  static std::ostream& Print(std::ostream& out, const FileDB& file_db);
};
#endif//LARTC_AST_FILE_DB
