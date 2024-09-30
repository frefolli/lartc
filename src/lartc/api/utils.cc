#include <lartc/api/utils.hh>
#include <lartc/api/config.hh>
#include <iostream>
#include <cstring>
#include <assert.h>

char* API::strclone(const char* string) {
  int len = strlen(string);
  char* buf = (char*) malloc (len + 1);
  strcpy(buf, string);
  return buf;
}

std::string API::generate_temp_file(std::string ext) {
  char buffer[L_tmpnam];
  std::string output_file = std::tmpnam(buffer);
  output_file = buffer;
  assert(!output_file.empty());
  output_file += ext;
  return output_file;
}

API::Result API::execute_command_line(std::string command_line) {
  if (API::ECHO_SYSTEM_COMMANDS) {
    std::clog << "|> \"" << command_line << "\"" << std::endl;
  }
  if (system(command_line.c_str()) == 0) {
    return API::Result::OK;
  }
  return API::Result::ERR;
}
