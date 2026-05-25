#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
namespace exprparse {

// clang-format off
#define EXPRPARSE_TOKENS\
  X(INVALID)\
  X(EOS)\
  X(IDENTIFIER)\
  X(INT_LIT)\
  X(FLOAT_LIT)\
  X(STRING_LIT)\
  X(NOT)\
  X(EQUALS)\
  X(EQUALS_EQUALS)\
  X(NOT_EQUALS)\
  X(LESS_THAN)\
  X(MORE_THAN)\
  X(LESS_OR_EQUAL)\
  X(MORE_OR_EQUAL)\
  X(OPEN_PARENT)\
  X(CLOSE_PARENT)\
  X(COMMA)\
  X(DOT_ACCESS)\
  X(PLUS)\
  X(PLUS_PLUS)\
  X(MINUS)\
  X(MINUS_MINUS)\
  X(SLASH)\
  X(STAR)\
  X(MOD)\
  X(PLUS_EQUALS)\
  X(MINUS_EQUALS)\
  X(SLASH_EQUALS)\
  X(STAR_EQUALS)\
  X(MOD_EQUALS)
// clang-format on

struct Box;
class ExprParse_Struct {
public:
  virtual ~ExprParse_Struct() = default;

  virtual std::string print() = 0;

  virtual Box get_member(std::string member) = 0;
  virtual void set_member(std::string member) = 0;
  virtual Box get_index(size_t idx) = 0;
  virtual void set_index(size_t idx) = 0;
};

class StringWrapper : public ExprParse_Struct {
public:
  std::string data;

  StringWrapper() = default;
  ~StringWrapper() = default;
  StringWrapper(std::string s);

  std::string print() override;

  Box get_member(std::string member) override;
  void set_member(std::string member) override;
  Box get_index(size_t idx) override;
  void set_index(size_t idx) override;
};

enum class tag {
  NONE,
  BOOL,
  INT,
  FLOAT,
  VOID_PTR,
  STRUCT_PTR,
};

struct Box {
  bool is_const = true;
  tag _tag = tag::NONE;
  union {
    bool _bool;
    int _int;
    float _float;
    void *void_ptr;
    ExprParse_Struct *struct_ptr;
  };
};

#define X(t) t,
enum class token_tag { EXPRPARSE_TOKENS };
#undef X

std::string tag2str(token_tag tag);

struct error_and_span {
  size_t start;
  size_t end;
  std::string error;
};

struct token {
  token_tag tag = token_tag::INVALID;
  int data_idx = -1;
};

// enum class tokeniser_state {
//   ordinary,
// };

class ExpresionParser {
private:
  // tokeniser_state mode = tokeniser_state::ordinary;
  void put_token_data(Box data);

public:
  token get_token();
  char *reader;
  error_and_span build_syntax_tree(std::string expr);
  bool run();

  std::vector<std::string> autocomplete_sources;
  std::unordered_map<std::string, Box> data;

  std::vector<Box> token_data;

private:
  std::string input;
  std::stringstream tmp;
};

} // namespace exprparse
