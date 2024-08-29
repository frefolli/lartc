project('lartc',
  ['c', 'cpp'],
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++20'])

tree_sitter = dependency('tree-sitter')
tree_sitter_lart = dependency('tree-sitter-lart')
include = include_directories('./include')

lartc_utils = library('lartc_utils', [
    'src/lartc/internal_errors.cc',
  ], include_directories: include)

lartc_declaration = library('lartc_declaration', [
    'src/lartc/ast/declaration/variants.cc',
    'src/lartc/ast/declaration/parse.cc',
    'src/lartc/ast/declaration.cc',
    'src/lartc/ast/type/variants.cc',
    'src/lartc/ast/type/parse.cc',
    'src/lartc/ast/type.cc'
  ], dependencies: [tree_sitter, tree_sitter_lart],
  link_with: [lartc_utils],
  include_directories: include)

executable('lartc', [
    'src/lartc/main.cc'
  ], dependencies: [tree_sitter, tree_sitter_lart],
  link_with: [lartc_declaration],
  include_directories: include)
