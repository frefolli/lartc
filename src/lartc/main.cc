#include <lartc/terminal.hh>
#include <lartc/api/lpp.hh>
#include <lartc/api/cpp.hh>
#include <lartc/api/llc.hh>
#include <lartc/api/as.hh>
#include <lartc/api/ld.hh>
#include <lartc/api/config.hh>

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <assert.h>
#include <cstring>
#include <iostream>

void print_help() {
  std::cout << "Usage: lartc [options] file..." << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "  -h/--help                Display this information." << std::endl;
  std::cout << "  -V/--version             Display compiler version information." << std::endl;
  std::cout << "" << std::endl;
  std::cout << "  -v                       Display the programs invoked by the compiler." << std::endl;
  std::cout << "  -E                       Preprocess only; do not compile, assemble or link." << std::endl;
  std::cout << "  -S                       Compile only; do not assemble or link." << std::endl;
  std::cout << "  -c                       Compile and assemble, but do not link." << std::endl;
  std::cout << "  -o/--output <file>       Place the output into <file>." << std::endl;
  std::cout << "" << std::endl;
  std::cout << "  -d                       Dumps debug information to stdout and to './tmp' directory." << std::endl;
  std::cout << "" << std::endl;
  std::cout << "For bug reporting, please see:" << std::endl;
  std::cout << "<https://github.com/frefolli/lartc/issues>." << std::endl;
}

int main(int argc, char** args) {
  std::vector<std::string> lart_files = {};
  std::vector<std::string> llvm_ir_files = {};
  std::vector<std::string> asm_files = {};
  std::vector<std::string> object_files = {};
  std::vector<std::string> c_files = {};

  std::string object_file;
  std::string asm_file;
  std::string llvm_ir_file;
  std::string linked_file;
  std::string output;

  enum Workflow {
    DONT_COMPILE, // -E
    DONT_ASSEMBLE,// -S
    DONT_LINK,    // -c
    ALL
  };
  Workflow workflow = ALL;

  uint64_t n_of_args = argc;
  for (uint64_t i = 1; i < n_of_args; ++i) {
    std::string arg = args[i];
    if (arg == "-d" || arg == "--debug") {
      API::DUMP_DEBUG_INFO_FOR_STRUCS = true;
      API::DEBUG_SEGFAULT_IDENTIFY_PHASE = true;
    } else if (arg == "-v" || arg == "--verbose") {
      API::ECHO_SYSTEM_COMMANDS = true;
    } else if (arg == "-h" || arg == "--help") {
      print_help();
      std::exit(0);
    } else if (arg == "-V" || arg == "--version") {
      std::exit(0);
    } else if (arg == "-E") {
      workflow = Workflow::DONT_COMPILE;
    } else if (arg == "-S") {
      workflow = Workflow::DONT_ASSEMBLE;
    } else if (arg == "-c") {
      workflow = Workflow::DONT_LINK;
    } else if (arg == "-o" || arg == "--output") {
      if (i + 1 < n_of_args) {
        output = args[i + 1];
        i += 1;
      } else {
        std::cerr << RED_TEXT << "error" << RED_TEXT << ": expected argument after '" << arg << "'" << std::endl;
        std::exit(1);
      }
    } else {
      std::string ext = std::filesystem::path(arg).extension();
      if (ext == ".lart") {
        lart_files.push_back(arg);
      } else if (ext == ".ll") {
        llvm_ir_files.push_back(arg);
      } else if (ext == ".s") {
        asm_files.push_back(arg);
      } else if (ext == ".h") {
        c_files.push_back(arg);
      } else if (ext == ".o") {
        object_files.push_back(arg);
      } else {
        std::cerr << RED_TEXT << "error" << RED_TEXT << ": file '" << arg << "' has unknown extension" << std::endl;
        std::exit(1);
      }
    }
  }

  if (c_files.size() > 0) {
    API::cpp(c_files);
  }

  if (lart_files.size() > 0) {
    if (workflow == Workflow::DONT_COMPILE) {
      if (output.empty()) {
        output = "a.ll";
      }
      llvm_ir_file = output;
    }

    API::lpp(lart_files, llvm_ir_file);
    llvm_ir_files.push_back(llvm_ir_file);
  }

  if (workflow != Workflow::DONT_COMPILE) {
    if (llvm_ir_files.size() > 0) {
      if (workflow == Workflow::DONT_ASSEMBLE) {
        if (output.empty()) {
          output = "a.s";
        }
        asm_file = output;
      }

      API::llc(llvm_ir_files, asm_file);
      asm_files.push_back(asm_file);
    }
  }

  if (workflow != Workflow::DONT_ASSEMBLE) {
    if (asm_files.size() > 0) {
      if (workflow == Workflow::DONT_LINK) {
        if (output.empty()) {
          output = "a.o";
        }
        object_file = output;
      }
      API::as(asm_files, object_file);
      object_files.push_back(object_file);
    }
  }

  if (workflow != Workflow::DONT_LINK) {
    if (output.empty()) {
      output = "a.exe";
    }
    linked_file = output;
    if (object_files.size() > 0) {
      API::ld(object_files, linked_file);
    }
  }
  return 0;
}
