#include <ExpresionParser.hpp>
namespace exprparse {

#define is_whitespace(c)                                                       \
  ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\0')
#define is_eoliteral(c)                                                        \
  ((c) == '\0' || (c) == '(' || (c) == ')' || (c) == ',' || (c) == '.' ||      \
   (c) == '!' || (c) == '<' || (c) == '>' || (c) == '=')
#define is_numeric(c) ((c) >= '0' && (c) <= '9')
#define is_ident_ascii(c)                                                      \
  (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))
#define is_ident_char(c)                                                       \
  (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))

token ExpresionParser::get_token() {
  bool can_be_num = false;
  bool can_be_float = false;
  bool can_be_ident = false;
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
  case '!':
    reader++;
    if (*reader == '=')
      return {token_tag::NOT_EQUALS};
    reader--;
    return {token_tag::NOT};
  case '=':
    reader++;
    if (*reader == '=')
      return {token_tag::EQUALS_EQUALS};
    reader--;
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
  }

  while (true) {
    if (is_numeric(*reader) && !can_be_ident) {
      tmp += *reader;
      can_be_num = can_be_num || !can_be_ident;
    } else if (is_ident_char(*reader) && !can_be_num) {
      tmp += *reader;
      can_be_ident = true;
    } else if (*reader == '.' && can_be_num && !can_be_float) {
      tmp += *reader;
      can_be_float = true;
    } else if (can_be_num &&
               (is_whitespace(*reader) || is_eoliteral(*reader))) {
      token tok{};
      if (can_be_float)
        tok = {token_tag::FLOAT_LIT};
      else
        tok = {token_tag::INT_LIT};
    } else if (can_be_ident &&
               (is_whitespace(*reader) || is_eoliteral(*reader))) {
      return {token_tag::STRING_LIT};
    } else {
      return {token_tag::INVALID};
    }
    reader++;
  }
  return {};
};

} // namespace exprparse
