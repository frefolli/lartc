project('lartc',
  ['c', 'cpp'],
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++20'])

tree_sitter = dependency('tree-sitter')
tree_sitter_lart = dependency('tree-sitter-lart')
include = include_directories('./include')

executable('lartc', [
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
    'src/lartc/ast/declaration.cc',
    'src/lartc/ast/symbol.cc',
    'src/lartc/ast/operator.cc',
    'src/lartc/ast/file_db.cc',
    'src/lartc/resolve/symbol_stack.cc',
    'src/lartc/resolve/symbol_cache.cc',
    'src/lartc/resolve/resolve_symbols.cc',
    'src/lartc/typecheck/type_cache.cc',
    'src/lartc/typecheck/check_types.cc',
    'src/lartc/main.cc'
  ], dependencies: [tree_sitter, tree_sitter_lart],
  include_directories: include)
