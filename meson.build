project('lartc',
  ['c', 'cpp'],
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++20'])

tree_sitter = dependency('tree-sitter')
tree_sitter_lart = dependency('tree-sitter-lart')

lartc = executable('lartc', [
  'src/main.cc'
], dependencies: [tree_sitter, tree_sitter_lart])
