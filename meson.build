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

lartc_type = library('lartc-type', [
    'src/lartc/ast/type/variants.cc',
    'src/lartc/ast/type/parse.cc',
    'src/lartc/ast/type.cc',
    'src/lartc/ast/parse.cc',
  ], dependencies: [tree_sitter, tree_sitter_lart],
  link_with: [lartc_utils],
  include_directories: include)

lartc_expr = library('lartc-expression', [
    'src/lartc/ast/expression/variants.cc',
    'src/lartc/ast/expression/parse.cc',
    'src/lartc/ast/expression.cc',
  ], dependencies: [tree_sitter, tree_sitter_lart],
  link_with: [lartc_utils, lartc_type],
  include_directories: include)

lartc_stmt = library('lartc-statement', [
    'src/lartc/ast/statement/variants.cc',
    'src/lartc/ast/statement/parse.cc',
    'src/lartc/ast/statement.cc',
  ], dependencies: [tree_sitter, tree_sitter_lart],
  link_with: [lartc_utils, lartc_expr, lartc_type],
  include_directories: include)

lartc_decl = library('lartc-declaration', [
    'src/lartc/ast/declaration/variants.cc',
    'src/lartc/ast/declaration/parse.cc',
    'src/lartc/ast/declaration.cc',
  ], dependencies: [tree_sitter, tree_sitter_lart],
  link_with: [lartc_utils, lartc_stmt, lartc_type],
  include_directories: include)

executable('lartc', [
    'src/lartc/main.cc'
  ], dependencies: [tree_sitter, tree_sitter_lart],
  link_with: [lartc_type, lartc_expr, lartc_stmt, lartc_decl],
  include_directories: include)
