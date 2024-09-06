#ifndef LARTC_TYPECHECK_EVALUATION
#define LARTC_TYPECHECK_EVALUATION
#include <lartc/ast/type.hh>
#include <lartc/resolve/symbol_cache.hh>

bool types_are_equal(Type* A, Type* B);
bool type_can_be_implicitly_casted_to(SymbolCache& symbol_cache, Type* src, Type* dst);
Type* resolve_type(SymbolCache& symbol_cache, Type* type);
#endif//LARTC_TYPECHECK_EVALUATION
