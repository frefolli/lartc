#ifndef LARTC__CODEGEN__MARKERS
#define LARTC__CODEGEN__MARKERS
#include <cstdint>
#include <string>
#include <unordered_map>
#include <lartc/ast/statement.hh>

enum marker_key {
  NONE_MK,
  CONTINUE_MK, BREAK_MK
};
struct Markers {
  uint64_t count = 2;
  std::unordered_map<marker_key, uint64_t> keyd;
  std::unordered_map<Statement*, uint64_t> vars;
  std::unordered_map<std::pair<std::string, Type*>*, uint64_t> params;

  inline std::string serialize(uint64_t marker) {
    return "%_" + std::to_string(marker);
  }

  std::string last_marker();
  std::string new_marker(marker_key key = NONE_MK);

  int64_t save_key(marker_key key);
  void restore_key(marker_key key, uint64_t old_key);
  std::string get_key(marker_key key);
  void no_key(marker_key key);

  void add_var(Statement* var);
  std::string get_var(Statement* var);
  void del_var(Statement* var);

  void add_param(std::pair<std::string, Type*>* param);
  std::string get_param(std::pair<std::string, Type*>* param);
  void clear_params();
};
#endif//LARTC__CODEGEN__MARKERS
