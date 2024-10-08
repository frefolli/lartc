project('lartc',
  ['c', 'cpp'],
  version : '0.1',
  default_options : ['warning_level=3', 'werror=true', 'cpp_std=c++20'])

tree_sitter = dependency('tree-sitter')
tree_sitter_lart = dependency('tree-sitter-lart')
tree_sitter_c = dependency('tree-sitter-c')
include = include_directories('./include')

executable('lartc', [
    'src/lartc/serializations.cc',
    'src/lartc/internal_errors.cc',
    'src/lartc/external_errors.cc',
    'src/lartc/ast/check.cc',
    'src/lartc/ast/parse.cc',
    'src/lartc/ast/type/variants.cc',
    'src/lartc/ast/type/parse.cc',
    'src/lartc/ast/type.cc',
    'src/lartc/ast/expression/variants.cc',
    'src/lartc/ast/expression/parse.cc',
    'src/lartc/ast/expression.cc',
    'src/lartc/ast/statement/variants.cc',
    'src/lartc/ast/statement/parse.cc',
    'src/lartc/ast/statement.cc',
    'src/lartc/ast/declaration/variants.cc',
    'src/lartc/ast/declaration/parse.cc',
    'src/lartc/ast/declaration/merge.cc',
    'src/lartc/ast/declaration.cc',
    'src/lartc/ast/symbol.cc',
    'src/lartc/ast/operator.cc',
    'src/lartc/ast/file_db.cc',
    'src/lartc/resolve/symbol_stack.cc',
    'src/lartc/resolve/symbol_cache.cc',
    'src/lartc/resolve/resolve_symbols.cc',
    'src/lartc/typecheck/type_cache.cc',
    'src/lartc/typecheck/check_types.cc',
    'src/lartc/typecheck/size_cache.cc',
    'src/lartc/typecheck/casting.cc',
    'src/lartc/typecheck/check_declared_types.cc',
    'src/lartc/codegen/emit_llvm.cc',
    'src/lartc/codegen/markers.cc',
    'src/lartc/codegen/literal_store.cc',
    'src/lartc/api/config.cc',
    'src/lartc/api/utils.cc',
    'src/lartc/api/lpp.cc',
    'src/lartc/api/cpp.cc',
    'src/lartc/api/llc.cc',
    'src/lartc/api/ld.cc',
    'src/lartc/api/as.cc',
    'src/lartc/main.cc'
  ], dependencies: [tree_sitter, tree_sitter_lart, tree_sitter_c],
  include_directories: include)
