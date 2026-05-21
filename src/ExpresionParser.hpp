#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <NanBoxing.hpp>

namespace exprparse {

enum EXPR_AUTOCOMPLETE_TYPE {
  EXPR_AC_FUNC,
  EXPR_AC_FIELD,
  EXPR_AC_ENUM,
};

class ExpresionParser {
  EXPR_AUTOCOMPLETE_TYPE current_autocomplete_type;
  std::vector<std::string> record_fields;
  std::unordered_map<std::string, std::vector<NanBox>> autocomplete_records;
};

}
