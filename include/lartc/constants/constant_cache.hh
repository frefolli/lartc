#ifndef LARTC_CONSTANTS_CONSTANT_CACHE
#define LARTC_CONSTANTS_CONSTANT_CACHE
#include <lartc/ast/expression.hh>
#include <lartc/ast/declaration.hh>
#include <lartc/resolve/symbol_cache.hh>
#include <map>

struct ConstantCache {
  std::map<Declaration*, Expression*> constants;
  std::map<Declaration*, bool> staging;

  static std::ostream& Print(std::ostream& out, ConstantCache& constant_cache);
  static void Delete(ConstantCache& constant_cache);
};
#endif//LARTC_CONSTANTS_CONSTANT_CACHE
