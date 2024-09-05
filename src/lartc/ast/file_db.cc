#include <lartc/ast/file_db.hh>
#include <cstring>
#include <tree_sitter/api.h>

inline char* read_source_code(const char* filepath) {
    char* text = NULL;
    FILE* file = fopen(filepath, "r");
    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);
    text = (char*) malloc(fsize + 1);
    fread(text, fsize, 1, file);
    fclose(file);
    return text;
}

FileDB::File* FileDB::add_file(const char* filepath) {
  files.push_back(FileDB::File {});
  FileDB::File* file = &files.back();
  file->filepath = filepath;
  file->source_code = read_source_code(file->filepath.c_str());
  return file;
}

void FileDB::add_symbol(Symbol* symbol, TSNode& node) {
  File* file = current_file();
  TSPoint point = ts_node_start_point(node);
  uint64_t byte_start = ts_node_start_byte(node);
  uint64_t byte_end = ts_node_end_byte(node);
  symbol_points[symbol] = FileDB::Point {
    .file = file,
    .row = point.row,
    .column = point.column,
    .byte_start = byte_start,
    .byte_end = byte_end
  };
}

void FileDB::add_expression(Expression* expression, TSNode& node) {
  File* file = current_file();
  TSPoint point = ts_node_start_point(node);
  uint64_t byte_start = ts_node_start_byte(node);
  uint64_t byte_end = ts_node_end_byte(node);
  expression_points[expression] = FileDB::Point {
    .file = file,
    .row = point.row,
    .column = point.column,
    .byte_start = byte_start,
    .byte_end = byte_end
  };
}

void FileDB::add_type(Type* type, TSNode& node) {
  File* file = current_file();
  TSPoint point = ts_node_start_point(node);
  uint64_t byte_start = ts_node_start_byte(node);
  uint64_t byte_end = ts_node_end_byte(node);
  type_points[type] = FileDB::Point {
    .file = file,
    .row = point.row,
    .column = point.column,
    .byte_start = byte_start,
    .byte_end = byte_end
  };
}

std::ostream& FileDB::File::Print(std::ostream& out, const FileDB::File& file) {
  out << file.filepath << " | " << (strlen(file.source_code)/1024) << " KB";
  return out;
}

std::ostream& FileDB::Point::Print(std::ostream& out, const FileDB::Point& point) {
  out << point.file->filepath << ":" << point.row << ":" << point.column;
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
    FileDB::Point::Print(out, symbol_point.second);
    out << std::endl;
  }

  out << "## Expression Points" << std::endl << std::endl;
  for (const auto& expression_point : file_db.expression_points) {
    out << " - ";
    Expression::Print(out, expression_point.first);
    out << " -> ";
    FileDB::Point::Print(out, expression_point.second);
    out << std::endl;
  }

  out << "## Type Points" << std::endl << std::endl;
  for (const auto& type_point : file_db.type_points) {
    out << " - ";
    Type::Print(out, type_point.first);
    out << " -> ";
    FileDB::Point::Print(out, type_point.second);
    out << std::endl;
  }
  return out;
}
