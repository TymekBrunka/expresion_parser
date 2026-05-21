#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <NanBoxing.hpp>

namespace exprparse {

enum class autocomplete_type {
  FUNC,
  FIELD,
  ENUM_STR,
};

class ExpresionParser {
  autocomplete_type current_autocomplete_type;
  std::unordered_map<std::string, std::vector<NanBox>> autocomplete_records;
};

}
