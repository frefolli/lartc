#ifndef LARTC_API_AS
#define LARTC_API_AS
#include <lartc/api/result.hh>
#include <string>
#include <vector>

namespace API {
  Result as(const std::vector<std::string>& asm_files, const std::vector<std::string>& arguments, const std::vector<std::string>& options, std::string& output_file);
}
#endif//LARTC_API_AS
