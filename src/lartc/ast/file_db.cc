#include <iostream>
#include <lartc/ast/file_db.hh>
#include <lartc/api/config.hh>
#include <cstring>
#include <tree_sitter/api.h>
#include <filesystem>
#include <lartc/terminal.hh>
#include <cassert>

inline char* read_source_code(const char* filepath) {
    char* text = NULL;
    FILE* file = fopen(filepath, "r");
    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);
    text = (char*) malloc(fsize + 1);
    text[fsize] = '\0';
    fsize = fread(text, fsize, 1, file);
    fclose(file);
    return text;
}

FileDB::Point FileDB::Point::From(const FileDB* file_db, TSNode& ts_node) {
  std::uintmax_t file = file_db->current_file_index();
  TSPoint point = ts_node_start_point(ts_node);
  std::uintmax_t byte_start = ts_node_start_byte(ts_node);
  std::uintmax_t byte_end = ts_node_end_byte(ts_node);
  return FileDB::Point {
    .file = file,
    .row = point.row,
    .column = point.column,
    .byte_start = byte_start,
    .byte_end = byte_end
  };
}

FileDB::File* FileDB::add_file(const char* filepath) {
  files.push_back(FileDB::File {});
  FileDB::File* file = current_file();
  file->filepath = "";
  file->filepath += filepath;
  file->source_code = read_source_code(file->filepath.c_str());
  return file;
}

void FileDB::add_symbol(Symbol* symbol, TSNode& node) {
  symbol_points[symbol] = FileDB::Point::From(this, node);
}

void FileDB::add_expression(Expression* expression, TSNode& node) {
  expression_points[expression] = FileDB::Point::From(this, node);
}

void FileDB::add_type(Type* type, TSNode& node) {
  type_points[type] = FileDB::Point::From(this, node);
}

void FileDB::add_declaration(Declaration* declaration, TSNode& node) {
  declaration_points[declaration] = FileDB::Point::From(this, node);
}

void FileDB::add_var(Statement* var_decl, TSNode& node) {
  var_decl_points[var_decl] = FileDB::Point::From(this, node);
}

std::ostream& FileDB::File::Print(std::ostream& out, const FileDB::File& file) {
  out << file.filepath << " | " << (strlen(file.source_code)/1024) << " KB";
  return out;
}

std::ostream& FileDB::Point::Print(std::ostream& out, const FileDB& file_db, const FileDB::Point& point) {
  out << file_db.files[point.file].filepath << ":" << point.row+1 << ":" << point.column+1;
  return out;
}

std::ostream& FileDB::Print(std::ostream& out, const FileDB& file_db) {
  out << "# FileDB" << std::endl << std::endl;

  out << "## Files" << std::endl << std::endl;
  for (const File& file : file_db.files) {
    FileDB::File::Print(out, file);
    out << std::endl;
  }

  out << "## Symbol Points" << std::endl << std::endl;
  for (const auto& symbol_point : file_db.symbol_points) {
    out << " - ";
    Symbol::Print(out, *symbol_point.first);
    out << " -> ";
    FileDB::Point::Print(out, file_db, symbol_point.second);
    out << std::endl;
  }

  out << "## Expression Points" << std::endl << std::endl;
  for (const auto& expression_point : file_db.expression_points) {
    out << " - ";
    Expression::Print(out, expression_point.first);
    out << " -> ";
    FileDB::Point::Print(out, file_db, expression_point.second);
    out << std::endl;
  }

  out << "## Type Points" << std::endl << std::endl;
  for (const auto& type_point : file_db.type_points) {
    out << " - ";
    Type::Print(out, type_point.first);
    out << " -> ";
    FileDB::Point::Print(out, file_db, type_point.second);
    out << std::endl;
  }

  out << "## Var Decl Points" << std::endl << std::endl;
  for (const auto& var_decl_point : file_db.var_decl_points) {
    out << " - ";
    Statement::PrintShort(out, var_decl_point.first);
    out << " -> ";
    FileDB::Point::Print(out, file_db, var_decl_point.second);
    out << std::endl;
  }
  return out;
}

std::string attempt_resolve(const std::filesystem::path& filepath, const std::filesystem::path& location) {
  assert(std::filesystem::is_directory(location));
  std::filesystem::path path = location/filepath;

  if (std::filesystem::exists(path))
    return path;

  if (!path.has_extension()) {
    path.replace_extension("lart");
    if (std::filesystem::exists(path))
      return path;
  }
  return "";
}

std::string FileDB::resolve_local(const std::string& filepath, const std::string& location) {
  std::string path = attempt_resolve(filepath, std::filesystem::path(location).remove_filename());
  if (path.empty())
    path = resolve_global(filepath);
  return path;
}

std::string FileDB::resolve_global(const std::string& filepath) {
  std::string path;
  for (std::string basepath : API::INCLUDE_DIRECTORIES) {
    path = attempt_resolve(filepath, basepath);
    if (!path.empty())
      break;
  }
  return path;
}
