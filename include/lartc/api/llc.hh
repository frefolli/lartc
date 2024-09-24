#ifndef LARTC_API_LLC
#define LARTC_API_LLC
#include <lartc/api/result.hh>
#include <string>
#include <vector>

namespace API {
  Result llc(const std::vector<std::string>& llvm_ir_files, const std::vector<std::string>& arguments, const std::vector<std::string>& options, std::string& output_file);
}
#endif//LARTC_API_LLC
