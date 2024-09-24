#ifndef LARTC_API_LD
#define LARTC_API_LD
#include <lartc/api/result.hh>
#include <string>
#include <vector>

namespace API {
  Result ld(const std::vector<std::string>& object_files, const std::vector<std::string>& arguments, const std::vector<std::string>& options, std::string& output_file);
}
#endif//LARTC_API_LD
