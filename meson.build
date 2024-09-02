project('lartc',
  ['c', 'cpp'],
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++20'])

tree_sitter = dependency('tree-sitter')
tree_sitter_lart = dependency('tree-sitter-lart')
include = include_directories('./include')

lartc_utils = library('lartc_utils', [
    'src/lartc/internal_errors.cc',
  ], dependencies: [tree_sitter, tree_sitter_lart],
  include_directories: include)

lartc_ast = library('lartc_ast', [
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
  ], dependencies: [tree_sitter, tree_sitter_lart],
  link_with: [lartc_utils],
  include_directories: include)

executable('lartc', [
    'src/lartc/main.cc'
  ], dependencies: [tree_sitter, tree_sitter_lart],
  link_with: [lartc_ast],
  include_directories: include)
