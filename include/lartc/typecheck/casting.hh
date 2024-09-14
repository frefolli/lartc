#ifndef LARTC_TYPECHECK_CASTING
#define LARTC_TYPECHECK_CASTING
#include <lartc/ast/type.hh>
#include <lartc/ast/declaration.hh>
#include <lartc/resolve/symbol_cache.hh>

std::pair<Type*, Declaration*> resolve_symbol_type(SymbolCache& symbol_cache, Declaration* context, Type* symbol_type);
bool types_are_namely_equal(SymbolCache& symbol_cache, Declaration* contextA, Type* A, Declaration* contextB, Type* B);
bool types_are_structurally_equal(SymbolCache& symbol_cache, Declaration* contextA, Type* A, Declaration* contextB, Type* B);
bool types_are_structurally_compatible(SymbolCache& symbol_cache, Declaration* contextSrc, Type* Src, Declaration* contextDst, Type* Dst);
bool type_can_be_implicitly_casted_to(SymbolCache& symbol_cache, Declaration* context, Type* src, Type* dst);
bool type_is_algebraically_manipulable(SymbolCache& symbol_cache, Declaration* context, Type* type);
bool types_are_algebraically_manipulable(SymbolCache& symbol_cache, Declaration* context, Type* A, Type* B);
bool type_is_logically_manipulable(SymbolCache& symbol_cache, Declaration* context, Type* type);
bool types_are_logically_manipulable(SymbolCache& symbol_cache, Declaration* context, Type* A, Type* B);
#endif//LARTC_TYPECHECK_CASTING
