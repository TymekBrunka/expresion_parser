#pragma once
#include <cstdint>
namespace exprparse {

uint64_t nanbox(double x);
uint64_t nanunbox(double x);

class NanBox {
private:
  uint64_t value;

public:
  NanBox() = default;

  template<typename T>
  NanBox(T x) {
    value = nanbox(x);
  }

  bool set_none();
  bool is_none() const;
  bool is_bool() const;
  bool is_nan() const;
  bool is_double() const;
  bool is_i32() const;
  bool is_ptr() const;
  bool is_ptr_to_struct() const;
};

} // namespace exprparse
