#ifndef LARTC__CODEGEN__EMIT_LLVM
#define LARTC__CODEGEN__EMIT_LLVM
#include <ostream>
#include <lartc/codegen/cg_context.hh>
#include <lartc/ast/declaration.hh>

void emit_llvm(std::ostream& out, CGContext& context, Declaration* decl_tree);
#endif//LARTC__CODEGEN__EMIT_LLVM
