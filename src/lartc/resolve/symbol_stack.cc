#include <lartc/resolve/symbol_stack.hh>

void SymbolStack::open_scope() {
  stack.push_back({});
}

void SymbolStack::close_scope() {
  stack.pop_back();
}

bool SymbolStack::set(std::string name, Type* type) {
  std::map<std::string, Type*>& frame = stack.front();
  auto it = frame.find(name);
  if (it != frame.end()) {
    return false;
  } else {
    frame[name] = type;
    return true;
  }
}

Type* SymbolStack::get(Symbol& symbol) {
  for (auto frame_it = stack.rbegin(); frame_it != stack.rend(); ++frame_it) {
    auto it = frame_it->find(symbol.identifiers.front());
    if (it != frame_it->end()) {
      return it->second;
    }
  }
  return nullptr;
}
