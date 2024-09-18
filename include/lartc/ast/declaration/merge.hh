#ifndef LARTC_AST_DECLARATION_MERGE
#define LARTC_AST_DECLARATION_MERGE
#include <lartc/tree_sitter.hh>

/** Returns a Declaration which is the fusion of both.
 *  Declarations passed to this function should be considered consumed and deleted.
 *  This function can have errors guided by:
 *  - 
 *  but by itself it doesn't fail, in case of inconsistences the older decl is returned.
  */
Declaration* merge_declarations(TSContext& context, Declaration* older, Declaration* latest);

#endif//LARTC_AST_DECLARATION_MERGE
