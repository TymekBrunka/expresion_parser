#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
namespace exprparse {

// uint64_t nan_box(double x) {
//   std::cout << "hello\n";
//   return 0;
// }
// uint64_t nan_unbox(double x);
//

class NanBox;

class ExprParse_Struct {
public:
  virtual ~ExprParse_Struct() = default;

  virtual uint64_t to_nanbox() noexcept;
  virtual uint64_t from_nanbox() noexcept;
  virtual NanBox get_member(std::string member);
  virtual void set_member(std::string member);
  virtual NanBox get_index(size_t idx);
  virtual void set_index(size_t idx);
};

enum class nanbox_type {
  DOUBLE,
  NAN,
  NULL_,
  BOOL,
  I32,
  VOID_PTR,
  STR_PTR,
  STRUCT_PTR,
};

const char* dbg_type(nanbox_type x);

class NanBox {
public:
  uint64_t value;

public:
  NanBox();
  NanBox(double x);
  NanBox(int32_t x);
  NanBox(bool x);
  NanBox(void* x);
  NanBox(std::string* x);
  NanBox(ExprParse_Struct* x);

  nanbox_type type() const;
  const char* type_name() const;
};

} // namespace exprparse
