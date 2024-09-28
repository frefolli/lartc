#include <lartc/api/ld.hh>
#include <lartc/api/utils.hh>
#include <lartc/api/config.hh>

#include <cstdio>
#include <cstdlib>
#include <lartc/ast/declaration.hh>
#include <lartc/ast/declaration/parse.hh>
#include <lartc/ast/check.hh>
#include <lartc/resolve/resolve_symbols.hh>
#include <lartc/tree_sitter.hh>
#include <lartc/terminal.hh>
#include <lartc/ast/file_db.hh>
#include <lartc/typecheck/type_cache.hh>
#include <lartc/typecheck/check_types.hh>
#include <lartc/typecheck/size_cache.hh>
#include <lartc/typecheck/check_declared_types.hh>
#include <lartc/codegen/cg_context.hh>
#include <lartc/codegen/emit_llvm.hh>

#include <assert.h>
#include <cstring>
#include <tree_sitter/api.h>

API::Result API::ld(const std::vector<std::string>& object_files, const std::vector<std::string>& arguments, const std::vector<std::string>& options, std::string& output_file) {
  if (output_file.empty()) {
    output_file = generate_temp_file(".exe");
  }

  std::ostringstream cmd ("");
  cmd << "clang";
  for (std::string object_file : object_files) {
    cmd << " " << object_file;
  }
  for (const std::string& argument : arguments) {
    cmd << " -Xlinker " << argument;
  }
  // i threw away -Wl but i feel safer to pass it to clang
  for (const std::string& option : options) {
    cmd << " -Wl" << option;
  }
  cmd << " -o " << output_file;
  
  if (execute_command_line(cmd.str()) == Result::OK) {
    return Result::OK;
  }
  return Result::LINKING_ERROR;
}
