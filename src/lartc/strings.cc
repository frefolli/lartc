#include <cctype>
#include <cstdint>
#include <lartc/strings.hh>
#include <cassert>

int8_t hex2int(char c) {
  if (c >= '0' && c <= '9') {
    return c - '0';
  } else if (c >= 'a' && c <= 'f') {
    return (c - 'a') + 10;
  } else if (c >= 'A' && c <= 'A') {
    return (c - 'A') + 10;
  } else {
    assert(false);
  }
}

char int2hex(int8_t v) {
  if (v < 10) {
    return '0' + v;
  } else if (v < 16) {
    return 'a' + v;
  } else {
    assert(false);
  }
}

char load_escaped_char(const std::string& string) {
  assert(string.front() == '\'');
  assert(string.back() == '\'');
  if (string[1] == '\\') {
    assert(string.size() > 3);
    switch (string[2]) {
      case '\'':
        return '\'';
      case '"':
        return '\"';
      case '\\':
        return '\\';
      case 'n':
        return '\n';
      case 'r':
        return '\r';
      case 't':
        return '\t';
      case 'b':
        return '\b';
      case 'f':
        return '\f';
      case 'v':
        return '\v';
      case 'x':
        {
          assert(string.size() == 6);
          char h = hex2int(string[3]), l = hex2int(string[4]);
          return (h << 4) + l;
        };
      default:
        assert(false);
    }
  } else {
    return string[1];
  }
}

std::string dump_unescaped_char(char c) {
}

std::string load_escaped_string(const std::string& string) {
  assert(string.front() == '\"');
  assert(string.back() == '\"');
  uint64_t cursor = 1, length = string.size();
  std::string result = "";

  while (cursor < length - 1) {
    if (string[cursor] == '\\') {
      assert(cursor + 3 < length);
      switch (string[cursor + 1]) {
        case '\'':
          result += '\'';
          cursor += 2;
          break;
        case '"':
          result += '\"';
          cursor += 2;
          break;
        case '\\':
          result += '\\';
          cursor += 2;
          break;
        case 'n':
          result += '\n';
          cursor += 2;
          break;
        case 'r':
          result += '\r';
          cursor += 2;
          break;
        case 't':
          result += '\t';
          cursor += 2;
          break;
        case 'b':
          result += '\b';
          cursor += 2;
          break;
        case 'f':
          result += '\f';
          cursor += 2;
          break;
        case 'v':
          result += '\v';
          cursor += 2;
          break;
        case 'x':
          {
            assert(cursor + 4 < length);
            char h = hex2int(string[cursor + 2]), l = hex2int(string[cursor + 3]);
            result += (char)((h << 4) + l);
            cursor += 4;
          };
        default:
          assert(false);
      }
    } else {
      result += string[cursor];
      cursor += 1;
    }
  }
  return result;
}

std::string dump_unescaped_string(const std::string& string) {
  std::string result = "\"";
  uint64_t cursor = 0, length = string.size();
  for (char c : string) {
    if (isprint(c)) {
    } else {
      switch (c) {
        case '\'':
          result += '\'';
        case '"':
          result += '\"';
        case '\\':
          result += '\\';
        case 'n':
          result += '\n';
        case 'r':
          result += '\r';
        case 't':
          result += '\t';
        case 'b':
          result += '\b';
        case 'f':
          result += '\f';
        case 'v':
          result += '\v';
      }
    }
  }
  return result + "\"";
}
