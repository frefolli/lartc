#ifndef LARTC_SERIALIZATIONS
#define LARTC_SERIALIZATIONS
#include <string>

char load_escaped_char(const std::string& string);
std::string dump_unescaped_char(char c);

std::string load_escaped_string(const std::string& string);
std::string dump_unescaped_string(const std::string& string, bool null_terminated = false);

bool load_boolean(const std::string& string);
std::string dump_boolean(bool value);
#endif//LARTC_SERIALIZATIONS
