#pragma once
#include <string>
#include <unordered_map>
#include <vector>

namespace exprparse {

struct Box;
class ExprParse_Struct {
public:
  virtual ~ExprParse_Struct() = default;

  virtual Box get_member(std::string member);
  virtual void set_member(std::string member);
  virtual Box get_index(size_t idx);
  virtual void set_index(size_t idx);
};

class StringWrapper : ExprParse_Struct {
public:
  std::string data;


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
  tag _tag;
  union {
    bool _bool;
    int _int;
    float _float;
    void *void_ptr;
    ExprParse_Struct *struct_ptr;
  };
};

enum class token_tag {
  INVALID = -1,
  EOS = 0,
  IDENTIFIER,
  INT_LIT,
  FLOAT_LIT,
  STRING_LIT,
  NOT,
  EQUALS,
  EQUALS_EQUALS,
  NOT_EQUALS,
  LESS_THAN,
  MORE_THAN,
  LESS_OR_EQUAL,
  MORE_OR_EQUAL,
  OPEN_PARENT,
  CLOSE_PARENT,
  COMMA,
  DOT_ACCESS,
};

struct error_and_span {
  size_t start;
  size_t end;
  std::string error;
};

struct token {
  token_tag tag = token_tag::INVALID;
  int data_idx = 0;
};

// enum class tokeniser_state {
//   ordinary,
// };

class ExpresionParser {
private:
  // tokeniser_state mode = tokeniser_state::ordinary;
  char *reader;
  token get_token();
  void put_token_data(Box data);

public:
  error_and_span build_syntax_tree(std::string expr);
  bool run();

  std::vector<std::string> autocomplete_sources;
  std::unordered_map<std::string, Box> data;

private:
  std::string input;
  std::string tmp;
  std::vector<Box> token_data;
};

} // namespace exprparse
