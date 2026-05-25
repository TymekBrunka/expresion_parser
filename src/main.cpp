#include <ExpresionParser.hpp>
#include <iostream>

int main() {
  std::string expr = "hi . hello ( cat != 12,\t67.89 / 34 *= ++ --  ) \"Hello\\t \\x1b[32mgeen\\x1b[0m World\\n \\x1b[31mhai\\x1b[0m \"  ";
  exprparse::ExpresionParser parser;
  parser.reader = expr.data();
  exprparse::token tok;
  std::vector<exprparse::token> tokens{};
  while (tok.tag != exprparse::token_tag::EOS ||
         parser.reader < &expr[expr.size()]) {
    tok = parser.get_token();
    std::cout << "char: " << (parser.reader - expr.data())
              << " tag: " << exprparse::tag2str(tok.tag) << "\n";
    tokens.push_back(tok);
  }

  for (auto &token : tokens) {
    exprparse::Box data{};
    if (token.data_idx != -1) {
      data = parser.token_data[token.data_idx];
    }
    std::stringstream _data{};
    if (data._tag == exprparse::tag::STRUCT_PTR)
      _data << data.struct_ptr->print();
    else if (data._tag == exprparse::tag::INT)
      _data << data._int;
    else if (data._tag == exprparse::tag::FLOAT)
      _data << data._float;
    std::cout << "tag: " << exprparse::tag2str(token.tag) << " data: " << _data.str() << "\n";
  }
}
