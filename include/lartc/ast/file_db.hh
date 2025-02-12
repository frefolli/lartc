#ifndef LARTC_AST_FILE_DB
#define LARTC_AST_FILE_DB
#include <cstdint>
#include <map>
#include <lartc/ast/symbol.hh>
#include <lartc/ast/expression.hh>
#include <lartc/ast/type.hh>
#include <lartc/ast/declaration.hh>
#include <tree_sitter/api.h>

struct FileDB {
  struct File {
    std::string filepath;
    char* source_code;

    static std::ostream& Print(std::ostream& out, const File& file);
  };
  struct Point {
    std::uintmax_t file;
    std::uintmax_t row;
    std::uintmax_t column;
    std::uintmax_t byte_start;
    std::uintmax_t byte_end;

    static std::ostream& Print(std::ostream& out, const FileDB& file_db, const Point& point);
    
    static Point From(const FileDB* file_db, TSNode& ts_node);
  };
  
  std::map<Symbol*, Point> symbol_points;
  std::map<Expression*, Point> expression_points;
  std::map<Type*, Point> type_points;
  std::map<Declaration*, Point> declaration_points;
  std::map<Statement*, Point> var_decl_points;
  std::vector<File> files;

  File* add_file(const char* filepath);
  void add_symbol(Symbol* symbol, TSNode& node);
  void add_expression(Expression* expression, TSNode& node);
  void add_type(Type* type, TSNode& node);
  void add_declaration(Declaration* declaration, TSNode& node);
  void add_var(Statement* var_decl, TSNode& node);
  inline std::uintmax_t current_file_index() const {
    return files.size() - 1;
  }
  inline File* current_file() {
    return files.data() + current_file_index();
  }

  static std::string resolve_local(const std::string& filepath, const std::string& location);
  static std::string resolve_global(const std::string& filepath);

  static std::ostream& Print(std::ostream& out, const FileDB& file_db);
};
#endif//LARTC_AST_FILE_DB
