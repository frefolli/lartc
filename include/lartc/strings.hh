#ifndef LARTC_STRINGS
#define LARTC_STRINGS
#include <string>

char load_escaped_char(const std::string& string);
std::string dump_unescaped_char(char c);

std::string load_escaped_string(const std::string& string);
std::string dump_unescaped_string(const std::string& string);
#endif//LARTC_STRINGS
