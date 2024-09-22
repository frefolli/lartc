#ifndef LARTC_API_LLC
#define LARTC_API_LLC
#include <string>
#include <vector>

namespace API {
  void llc(const std::vector<std::string>& llvm_ir_files, std::string& output_file);
}
#endif//LARTC_API_LLC
