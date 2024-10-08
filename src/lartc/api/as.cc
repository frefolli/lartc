#include <lartc/api/as.hh>
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

API::Result API::as(const std::vector<std::string>& asm_files, const std::vector<std::string>& arguments, const std::vector<std::string>& options, std::string& output_file) {
  if (output_file.empty()) {
    output_file = generate_temp_file(".o");
  }

  std::ostringstream cmd ("");
  cmd << "as";
  for (std::string asm_file : asm_files) {
    cmd << " " << asm_file;
  }
  for (const std::string& argument : arguments) {
    cmd << " " << argument;
  }
  for (const std::string& option : options) {
    cmd << " " << option;
  }
  cmd << " -o " << output_file;
  
  if (execute_command_line(cmd.str()) == Result::OK) {
    return Result::OK;
  }
  return Result::ASSEMBLING_ERROR;
}
