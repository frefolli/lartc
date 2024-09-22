#ifndef LARTC_API_LPP
#define LARTC_API_LPP
#include <lartc/api/result.hh>
#include <vector>
#include <string>

namespace API {
  void lpp(const std::vector<std::string>& lart_files, std::string& output_file);
}
#endif//LARTC_API_LPP
