#include <lartc/codegen/markers.hh>

  std::string Markers::last_marker() {
    return serialize(count);
  }

  std::string Markers::new_marker(marker_key key) {
    uint64_t marker = count++;
    if (key != NONE_MK) {
      keyd[key] = marker;
    }
    return serialize(marker);
  }

  int64_t Markers::save_key(marker_key key) {
    if (keyd.contains(key)) {
      return keyd[key];
    }
    return -1;
  }

  void Markers::restore_key(marker_key key, uint64_t old_key) {
    keyd[key] = old_key;
  }

  std::string Markers::get_key(marker_key key) {
    if (keyd.contains(key)) {
      return serialize(keyd[key]);
    }
    return "";
  }

  void Markers::no_key(marker_key key) {
    if (keyd.contains(key)) {
      keyd.erase(key);
    }
  }

  void Markers::add_var(Statement* var) {
    uint64_t marker = count++;
    vars[var] = marker;
  }
  
  std::string Markers::get_var(Statement* var) {
    if (vars.contains(var)) {
      return serialize(vars[var]);
    }
    return "";
  }
  
  void Markers::del_var(Statement* var) {
    if (vars.contains(var)) {
      vars.erase(var);
    }
  }
