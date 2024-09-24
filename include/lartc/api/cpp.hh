#ifndef LARTC_API_CPP
#define LARTC_API_CPP
#include <lartc/api/result.hh>
#include <vector>
#include <string>

namespace API {
  Result cpp(const std::vector<std::string>& c_files);
}
#endif//LARTC_API_CPP
