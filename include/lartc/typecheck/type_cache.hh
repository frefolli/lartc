#ifndef LARTC_TYPECHECK_TYPE_CACHE
#define LARTC_TYPECHECK_TYPE_CACHE
#include <lartc/ast/expression.hh>
#include <lartc/ast/type.hh>
#include <map>

struct TypeCache {
  std::map<Expression*, Type*> expression_types;

  static std::ostream& Print(std::ostream& out, TypeCache& type_cache);
  static void Delete(TypeCache& type_cache);
};
#endif//LARTC_TYPECHECK_TYPE_CACHE

