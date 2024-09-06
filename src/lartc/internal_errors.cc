#include <lartc/internal_errors.hh>
#include <iostream>

std::ostream& operator<<(std::ostream& out, internal_error_t error) {
  constexpr const char* _variants[] = {
    #define X(_) #_
      INTERNAL_ERROR_VARIANTS
    #undef X
  };
  constexpr int _bound = (sizeof(_variants) / sizeof(const char *));

  // Int values can be coerced to internal_error_t
  if (error < _bound)
    return out << _variants[error];
  return out << "internal error occurred, but I'm unable to display which one";
}

void throw_internal_error(internal_error_t error, std::stringstream msg) {
  std::cerr << error << msg.str() << std::endl;
  std::exit(error);
}

void throw_internal_error(internal_error_t error, std::string msg) {
  std::cerr << error << msg << std::endl;
  std::exit(error);
}
