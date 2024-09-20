#ifndef LARTC__CODEGEN__CG_CONTEXT
#define LARTC__CODEGEN__CG_CONTEXT
#include <lartc/ast/file_db.hh>
#include <lartc/resolve/symbol_cache.hh>
#include <lartc/typecheck/type_cache.hh>
#include <lartc/typecheck/size_cache.hh>
#include <lartc/codegen/literal_store.hh>

struct CGContext {
  FileDB& file_db;
  SymbolCache& symbol_cache;
  TypeCache& type_cache;
  SizeCache& size_cache;
  LiteralStore& literal_store;
};
#endif//LARTC__CODEGEN__CG_CONTEXT
