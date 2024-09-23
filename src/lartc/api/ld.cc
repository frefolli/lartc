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

void API::ld(const std::vector<std::string>& object_files, std::string& output_file) {
  if (output_file.empty()) {
    output_file = generate_temp_file(".exe");
  }

  std::ostringstream cmd ("");
  // cmd << "/usr/bin/ld --hash-style=gnu --build-id --eh-frame-hdr -m elf_x86_64 -dynamic-linker /lib64/ld-linux-x86-64.so.2 /usr/bin/../lib/gcc/x86_64-redhat-linux/14/../../../../lib64/crt1.o /usr/bin/../lib/gcc/x86_64-redhat-linux/14/../../../../lib64/crti.o /usr/bin/../lib/gcc/x86_64-redhat-linux/14/crtbegin.o -L/usr/bin/../lib/clang/18/lib/x86_64-redhat-linux-gnu -L/usr/bin/../lib/gcc/x86_64-redhat-linux/14 -L/usr/bin/../lib/gcc/x86_64-redhat-linux/14/../../../../lib64 -L/lib/../lib64 -L/usr/lib/../lib64 -L/lib -L/usr/lib -lgcc --as-needed -lgcc_s --no-as-needed -lc -lgcc --as-needed -lgcc_s --no-as-needed /usr/bin/../lib/gcc/x86_64-redhat-linux/14/crtend.o /usr/bin/../lib/gcc/x86_64-redhat-linux/14/../../../../lib64/crtn.o";
  cmd << "clang";
  for (std::string object_file : object_files) {
    cmd << " " << object_file;
  }
  cmd << " -o " << output_file;
  
  assert(execute_command_line(cmd.str()) == API::Result::OK);
}
