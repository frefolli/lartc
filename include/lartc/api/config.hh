#ifndef LARTC_API_CONFIG
#define LARTC_API_CONFIG

#include <cstdint>
#include <vector>
#include <string>

namespace API {
  extern bool DEBUG_SEGFAULT_IDENTIFY_PHASE;
  extern bool DUMP_DEBUG_INFO_FOR_STRUCS;
  extern bool ECHO_SYSTEM_COMMANDS;
  constexpr std::uintmax_t CPU_BIT_SIZE = sizeof(void*) * 8;
  constexpr std::uintmax_t STRUCT_PASSED_AS_INLINE_SIZE_LIMIT = 128;
  extern std::vector<std::string> INCLUDE_DIRECTORIES;
}
#endif//LARTC_API_CONFIG
