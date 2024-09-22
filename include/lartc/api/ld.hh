#ifndef LARTC_API_LD
#define LARTC_API_LD
#include <string>
#include <vector>

namespace API {
  void ld(const std::vector<std::string>& object_files, std::string& output_file);
}
#endif//LARTC_API_LD
