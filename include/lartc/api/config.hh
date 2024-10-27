#ifndef LARTC_API_CONFIG
#define LARTC_API_CONFIG

#include <cstdint>
namespace API {
  extern bool DEBUG_SEGFAULT_IDENTIFY_PHASE;
  extern bool DUMP_DEBUG_INFO_FOR_STRUCS;
  extern bool ECHO_SYSTEM_COMMANDS;
  constexpr uint64_t CPU_BIT_SIZE = sizeof(void*) * 8;
  constexpr uint64_t STRUCT_PASSED_AS_INLINE_SIZE_LIMIT = 128;
}
#endif//LARTC_API_CONFIG
