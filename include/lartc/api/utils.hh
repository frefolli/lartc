#ifndef LARTC_API_UTILS
#define LARTC_API_UTILS
#include <lartc/api/result.hh>
#include <string>

namespace API {
  char* strclone(const char* string);
  std::string generate_temp_file(std::string ext);
  API::Result execute_command_line(std::string command_line);
}
#endif//LARTC_API_UTILS
