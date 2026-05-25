#include <ExpresionParser.hpp>
#include <iostream>
namespace exprparse {

StringWrapper::StringWrapper(std::string s) { data = s; }
std::string StringWrapper::print() { return data; }
Box StringWrapper::get_member(std::string member) { return {}; }
void StringWrapper::set_member(std::string member) {}
Box StringWrapper::get_index(size_t idx) { return {}; }
void StringWrapper::set_index(size_t idx) {}

#define X(t)                                                                   \
  case token_tag::t:                                                           \
    return std::string(#t);

std::string tag2str(token_tag tag) {
  switch (tag) { EXPRPARSE_TOKENS }
}
#undef X

#define is_whitespace(c)                                                       \
  ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\0')
#define is_eoliteral(c)                                                        \
  ((c) == '\0' || (c) == '(' || (c) == ')' || (c) == ',' || (c) == '.' ||      \
   (c) == '!' || (c) == '<' || (c) == '>' || (c) == '=' || (c) == '+' ||       \
   (c) == '-' || (c) == '/' || (c) == '%' || (c) == '"')
#define is_string_escape(c)                                                    \
  ((c) == '\\' || (c) == '"' || (c) == 'b' || (c) == 'a' || (c) == 'f' ||      \
   (c) == 'n' || (c) == 'r' || (c) == 't' || (c) == 'x' || (c) == 'X')
#define say_gex(c)                                                             \
  (((c) >= '0' && (c) <= '9') || ((c) >= 'a' && (c) <= 'f') ||                 \
   ((c) >= 'A' && (c) <= 'F'))
#define gay_hex(c)                                                             \
  (((c) >= '0' && (c) <= '9')                                                  \
       ? (c) - '0'                                                             \
       : (((c) >= 'a' && (c) <= 'f') ? (c) - 'a' + 10 : (c) - 'A' + 10))

#define is_numeric(c) ((c) >= '0' && (c) <= '9')
#define is_ident_ascii(c)                                                      \
  (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))
#define is_ident_char(c)                                                       \
  (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))

token ExpresionParser::get_token() {
  bool can_be_num = false;
  bool can_be_float = false;
  bool can_be_ident = false;

  while (is_whitespace(*reader) && *reader != '\0')
    reader++;

  switch (*reader) {

  case '\0':
    return {token_tag::EOS};
  case '(':
    reader++;
    return {token_tag::OPEN_PARENT};
  case ')':
    reader++;
    return {token_tag::CLOSE_PARENT};
  case ',':
    reader++;
    return {token_tag::COMMA};
  case '.':
    reader++;
    return {token_tag::DOT_ACCESS};
  case '+':
    reader++;
    if (*reader == '=') {
      reader++;
      return {token_tag::PLUS_EQUALS};
    } else if (*reader == '+') {
      reader++;
      return {token_tag::PLUS_PLUS};
    }
    return {token_tag::PLUS};
  case '-':
    reader++;
    if (*reader == '=') {
      reader++;
      return {token_tag::MINUS_EQUALS};
    } else if (*reader == '-') {
      reader++;
      return {token_tag::MINUS_MINUS};
    }
    return {token_tag::MINUS};
  case '/':
    reader++;
    if (*reader == '=') {
      reader++;
      return {token_tag::SLASH_EQUALS};
    }
    return {token_tag::SLASH};
  case '*':
    reader++;
    if (*reader == '=') {
      reader++;
      return {token_tag::STAR_EQUALS};
    }
    return {token_tag::STAR};
  case '%':
    reader++;
    if (*reader == '=') {
      reader++;
      return {token_tag::MOD_EQUALS};
    }
    return {token_tag::MOD};
  case '!':
    reader++;
    if (*reader == '=') {
      reader++;
      return {token_tag::NOT_EQUALS};
    }
    return {token_tag::NOT};
  case '=':
    reader++;
    if (*reader == '=') {
      reader++;
      return {token_tag::EQUALS_EQUALS};
    }
    return {token_tag::EQUALS};
  case '<':
    reader++;
    if (*reader == '=')
      return {token_tag::LESS_OR_EQUAL};
    reader--;
    return {token_tag::LESS_THAN};
  case '>':
    reader++;
    if (*reader == '=')
      return {token_tag::MORE_OR_EQUAL};
    reader--;
    return {token_tag::MORE_THAN};
  case '"':
    reader++;
    while (true) {
      if (*reader == '\n') {
        reader++;
        continue;
      } else if (*reader == '\\') {
        reader++;
        if (is_string_escape(*reader)) {
          switch (*reader) {
          case '\\':
            tmp << '\\';
            break;
          case '"':
            tmp << '\"';
            break;
          case 'a':
            tmp << '\a';
            break;
          case 'b':
            tmp << '\b';
            break;
          case 'f':
            tmp << '\f';
            break;
          case 'n':
            tmp << '\n';
            break;
          case 'r':
            tmp << '\r';
            break;
          case 't':
            tmp << '\t';
            break;
          case 'x':
          case 'X':
            reader++;
            while (say_gex(*reader) && say_gex(*(reader + 1))) {
              tmp << (char)((16 * gay_hex(*reader)) + gay_hex(*(reader + 1)));
              reader += 2;
            }
            reader-=2;
            break;
          default:
            return {token_tag::INVALID};
          }
          reader++;
        }
      } else if (*reader == '"') {
        reader++;
        auto *sw = new StringWrapper(tmp.str());
        tmp.str(std::string());
        Box box = {._tag = tag::STRUCT_PTR, .struct_ptr = sw};
        token_data.push_back(box);
        token tok = {token_tag::STRING_LIT};
        tok.data_idx = token_data.size() - 1;
        return tok;
      } else if (*reader == '\0') {
        reader--;
        return {token_tag::INVALID};
      } else {
        tmp << *reader;
      }
      reader++;
    }
  }

  while (true) {
    if (is_numeric(*reader) && !can_be_ident) {
      tmp << *reader;
      // can_be_num = can_be_num || !can_be_ident;
      can_be_num = true;
    } else if (is_ident_char(*reader) && !can_be_num) {
      tmp << *reader;
      can_be_ident = true;
    } else if (*reader == '.' && can_be_num && !can_be_float) {
      tmp << *reader;
      can_be_float = true;
    } else if (can_be_num &&
               (is_whitespace(*reader) || is_eoliteral(*reader))) {
      token tok{};
      if (can_be_float) {
        Box box = {._tag = tag::FLOAT,
                   ._float = (float)atof(tmp.str().c_str())};
        tmp.str(std::string());
        token_data.push_back(box);
        tok = {token_tag::FLOAT_LIT};
        tok.data_idx = token_data.size() - 1;
        return tok;
      } else {
        Box box = {._tag = tag::INT, ._int = atoi(tmp.str().c_str())};
        tmp.str(std::string());
        token_data.push_back(box);
        tok = {token_tag::INT_LIT};
        tok.data_idx = token_data.size() - 1;
        return tok;
      }
    } else if (can_be_ident &&
               (is_whitespace(*reader) || is_eoliteral(*reader))) {
      auto *sw = new StringWrapper(tmp.str());
      tmp.str(std::string());
      Box box = {._tag = tag::STRUCT_PTR, .struct_ptr = sw};
      token_data.push_back(box);
      token tok = {token_tag::IDENTIFIER};
      tok.data_idx = token_data.size() - 1;
      return tok;
    } else {
      reader++;
      return {token_tag::INVALID};
    }
    reader++;
  }
  return {};
};

} // namespace exprparse
