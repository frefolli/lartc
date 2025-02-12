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
  std::cout << "  -Wg,<options>            Pass comma-separated <options> on to the generator." << std::endl;
  std::cout << "  -Wa,<options>            Pass comma-separated <options> on to the assembler." << std::endl;
  std::cout << "  -Wl,<options>            Pass comma-separated <options> on to the linker." << std::endl;
  std::cout << "  -Xgenerator <arg>        Pass <arg> on to the generator." << std::endl;
  std::cout << "  -Xassembler <arg>        Pass <arg> on to the assembler." << std::endl;
  std::cout << "  -Xlinker <arg>           Pass <arg> on to the linker." << std::endl;
  std::cout << "" << std::endl;
  std::cout << "  Passing *.lart, *.ll, *.s, *.o, *.a, *.so files will cause them to be included in the correct phase of the compilation." << std::endl;
  std::cout << "  As an experimental feature, passing *.h files will cause the compiler to parse those C-files and try to convert them to LART code (intended for headers). The result is printed to stdout, then the execution terminates." << std::endl;
  std::cout << "" << std::endl;
  std::cout << "For bug reporting, please see:" << std::endl;
  std::cout << "<https://github.com/frefolli/lartc/issues>." << std::endl;
}

std::string read_next_arg(char** args, std::uintmax_t n_of_args, std::uintmax_t& arg_index, const char* embedding_prefix = nullptr) {
  std::string flag = args[arg_index];
  if (embedding_prefix != nullptr) {
    std::string prefix = embedding_prefix;
    if (flag.starts_with(prefix) && flag.size() > prefix.size()) {
      return flag.substr(prefix.size());
    }
  }
  if (arg_index + 1 < n_of_args) {
    arg_index  += 1;
    return args[arg_index];
  } else {
    std::cerr << RED_TEXT << "error" << RED_TEXT << ": expected argument after '" << flag << "'" << NORMAL_TEXT << std::endl;
    std::exit(1);
  }
  assert(false);
}

void print_error(API::Result error) {
  switch(error) {
    case API::Result::OK:
      {
        assert(false);
      }
    case API::Result::ERR:
      {
        std::cerr << RED_TEXT << "generic error" << NORMAL_TEXT << std::endl;
        break;
      }
    case API::Result::NO_SOURCE_FILE_SPECIFIED:
      {
        std::cerr << RED_TEXT << "no source file specified" << NORMAL_TEXT << std::endl;
        break;
      }
    case API::Result::PARSING_ERROR:
      {
        std::cerr << RED_TEXT << "parsing error" << NORMAL_TEXT << std::endl;
        break;
      }
    case API::Result::SYMBOL_RESOLUTION_ERROR:
      {
        std::cerr << RED_TEXT << "symbol resolution error" << NORMAL_TEXT << std::endl;
        break;
      }
    case API::Result::DECLARED_TYPE_CHECKING_ERROR:
      {
        std::cerr << RED_TEXT << "declared type checking error" << NORMAL_TEXT << std::endl;
        break;
      }
    case API::Result::TYPE_CHECKING_ERROR:
      {
        std::cerr << RED_TEXT << "type checking error" << NORMAL_TEXT << std::endl;
        break;
      }
    case API::Result::LLVM_IR_GENERATION_ERROR:
      {
        std::cerr << RED_TEXT << "llvm ir generation error" << NORMAL_TEXT << std::endl;
        break;
      }
    case API::Result::ASM_GENERATION_ERROR:
      {
        std::cerr << RED_TEXT << "asm generation error" << NORMAL_TEXT << std::endl;
        break;
      }
    case API::Result::ASSEMBLING_ERROR:
      {
        std::cerr << RED_TEXT << "assembling error" << NORMAL_TEXT << std::endl;
        break;
      }
    case API::Result::LINKING_ERROR:
      {
        std::cerr << RED_TEXT << "linking error" << NORMAL_TEXT << std::endl;
        break;
      }
  }
}

void ensure_success(API::Result result) {
  if (result != API::Result::OK) {
    std::cerr << "cause of exit: ";
    print_error(result);
    std::exit(1);
  }
}

std::vector<std::string> lart_file_extensions = {".lart"};
std::vector<std::string> llvm_ir_file_extensions = {".ll", ".bc"};
std::vector<std::string> object_file_extensions = {".o", ".a", ".so"};
std::vector<std::string> asm_file_extensions = {".s"};
std::vector<std::string> c_file_extensions = {".c", ".h"};

bool match_extension(std::string ext, std::vector<std::string>& extensions) {
  for (const std::string& comparable : extensions) {
    if (comparable == ext) {
      return true;
    }
  }
  return false;
}

int main(int argc, char** args) {
  std::vector<std::string> lart_files = {};
  std::vector<std::string> llvm_ir_files = {};
  std::vector<std::string> asm_files = {};
  std::vector<std::string> object_files = {};
  std::vector<std::string> c_files = {};

  std::vector<std::string> generator_options = {};
  std::vector<std::string> generator_args = {};
  std::vector<std::string> assembler_options = {};
  std::vector<std::string> assembler_args = {};
  std::vector<std::string> linker_options = {};
  std::vector<std::string> linker_args = {};

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

  std::uintmax_t n_of_args = argc;
  for (std::uintmax_t i = 1; i < n_of_args; ++i) {
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
    } else if (arg.starts_with("-Wg")) {
      std::string options = read_next_arg(args, n_of_args, i, "-Wg");
      generator_options.push_back(options);
    } else if (arg.starts_with("-Wa")) {
      std::string options = read_next_arg(args, n_of_args, i, "-Wa");
      assembler_options.push_back(options);
    } else if (arg.starts_with("-Wl")) {
      std::string options = read_next_arg(args, n_of_args, i, "-Wl");
      linker_options.push_back(options);
    } else if (arg == "-Xgenerator") {
      std::string argument = read_next_arg(args, n_of_args, i);
      generator_args.push_back(argument);
    } else if (arg == "-Wassembler") {
      std::string argument = read_next_arg(args, n_of_args, i);
      assembler_args.push_back(argument);
    } else if (arg == "-Xlinker") {
      std::string argument = read_next_arg(args, n_of_args, i);
      linker_args.push_back(argument);
    } else if (arg == "-o" || arg == "--output") {
      output = read_next_arg(args, n_of_args, i);
    } else if (arg.starts_with("-l") || arg == "--link") {
      std::string library = ("-l" + read_next_arg(args, n_of_args, i, "-l"));
      linker_args.push_back(library);
    } else {
      std::string ext = std::filesystem::path(arg).extension();
      if (match_extension(ext, lart_file_extensions)) {
        lart_files.push_back(arg);
      } else if (match_extension(ext, llvm_ir_file_extensions)) {
        llvm_ir_files.push_back(arg);
      } else if (match_extension(ext, asm_file_extensions)) {
        asm_files.push_back(arg);
      } else if (match_extension(ext, c_file_extensions)) {
        c_files.push_back(arg);
      } else if (match_extension(ext, object_file_extensions)) {
        object_files.push_back(arg);
      } else {
        std::cerr << RED_TEXT << "error" << RED_TEXT << ": file '" << arg << "' has unknown extension" << NORMAL_TEXT << std::endl;
        std::exit(1);
      }
    }
  }

  if (c_files.size() > 0) {
    ensure_success(API::cpp(c_files));
    std::exit(0);
  }

  if (lart_files.size() > 0) {
    if (workflow == Workflow::DONT_COMPILE) {
      if (output.empty()) {
        output = "a.ll";
      }
      llvm_ir_file = output;
    }

    ensure_success(API::lpp(lart_files, llvm_ir_file));
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

      ensure_success(API::llc(llvm_ir_files, generator_args, generator_options, asm_file));
      asm_files.push_back(asm_file);
    }

    if (workflow != Workflow::DONT_ASSEMBLE) {
      if (asm_files.size() > 0) {
        if (workflow == Workflow::DONT_LINK) {
          if (output.empty()) {
            output = "a.o";
          }
          object_file = output;
        }
        ensure_success(API::as(asm_files, assembler_args, assembler_options, object_file));
        object_files.push_back(object_file);
      }

      if (workflow != Workflow::DONT_LINK) {
        if (output.empty()) {
          output = "a.exe";
        }
        linked_file = output;
        if (object_files.size() > 0) {
          ensure_success(API::ld(object_files, linker_args, linker_options, linked_file));
        }
      }
    }
  }
  return 0;
}
