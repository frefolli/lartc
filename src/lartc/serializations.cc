#include <cctype>
#include <cstdint>
#include <iostream>
#include <lartc/serializations.hh>
#include <lartc/terminal.hh>
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
    return 'a' + (v - 10);
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
      case '0':
        return '\0';
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
  std::string result = "\'";
  if (isprint(c)) {
    result += c;
  } else {
    switch (c) {
      case '\0':
        result += "\\0";
        break;
      case '\'':
        result += "\\'";
        break;
      case '"':
        result += "\\\"";
        break;
      case '\\':
        result += "\\\\";
        break;
      case '\n':
        result += "\\n";
        break;
      case '\r':
        result += "\\r";
        break;
      case '\t':
        result += "\\t";
        break;
      case '\b':
        result += "\\b";
        break;
      case '\f':
        result += "\\f";
        break;
      case '\v':
        result += "\\v";
        break;
      default:
        result += "\\x";
        result += int2hex(c >> 4);
        result += int2hex(c & 15);
    }
  }
  return result + "\'";
}

std::string load_escaped_string(const std::string& string) {
  assert(string.front() == '\"');
  assert(string.back() == '\"');
  std::uintmax_t cursor = 1, length = string.size();
  std::string result = "";

  while (cursor < length - 1) {
    if (string[cursor] == '\\') {
      switch (string[cursor + 1]) {
        case '0':
          result += '\0';
          cursor += 2;
          break;
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
            break;
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

std::string dump_unescaped_string(const std::string& string, bool null_terminated) {
  std::string result = "\"";
  for (char c : string) {
    if (c == '"') {
      result += "\22";
    } else if (c == '\\') {
      result += "\\\\";
    } else if (isprint(c)) {
      result += c;
    } else {
      char h = int2hex(c >> 4);
      char l = int2hex(c & 15);
      result += "\\";
      result += h;
      result += l;
    }
  }
  if (null_terminated) {
    result += "\\00";
  }
  return result + "\"";
}

bool load_boolean(const std::string& string) {
  if (string == "true") {
    return true;
  } else if (string == "false") {
    return false;
  }
  assert(false);
}

std::string dump_boolean(bool value) {
  if (value) {
    return "true";
  } else {
    return "false";
  }
}
