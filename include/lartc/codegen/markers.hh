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

  uint64_t count = 0;
  std::unordered_map<marker_key, uint64_t> keyd;
  std::unordered_map<Statement*, uint64_t> vars;

  inline std::string serialize(uint64_t marker) {
    return "%" + std::to_string(marker);
  }

  std::string last_marker() {
    return serialize(count);
  }

  std::string new_marker(marker_key key = NONE_MK) {
    if (key != NONE_MK) {
      keyd[key] = count;
    }
    return serialize(++count);
  }

  int64_t save_key(marker_key key) {
    if (keyd.contains(key)) {
      return keyd[key];
    }
    return -1;
  }

  void restore_key(marker_key key, uint64_t old_key) {
    keyd[key] = old_key;
  }

  std::string get_key(marker_key key) {
    if (keyd.contains(key)) {
      return serialize(keyd[key]);
    }
    return "";
  }

  void no_key(marker_key key) {
    if (keyd.contains(key)) {
      keyd.erase(key);
    }
  }

  void add_var(Statement* var) {
    vars[var] = count;
    ++count;
  }
  
  std::string get_var(Statement* var) {
    if (vars.contains(var)) {
      return serialize(vars[var]);
    }
    return "";
  }
  
  void del_var(Statement* var) {
    if (vars.contains(var)) {
      vars.erase(var);
    }
  }
};
#endif//LARTC__CODEGEN__MARKERS
