#include <NanBoxing.hpp>
#include <cstring>
namespace exprparse {

#define left_aligned(v, x) &((uint8_t *)&(v))[8 - sizeof((x))]

#define NAN ((uint64_t)0x7ffc000000000000)  // <62; 50>
#define QNAN ((uint64_t)0x0004000000000000) // 1 << 50 // 51st bit

#define TAG_MASK ((uint64_t)0x8001800000000000) // bits 48 and 49
#define IS_BOOL_MASK ((uint64_t)0x0001000000000000)

#define B_NAN ((uint64_t)0x0000800000000000)
#define B_NULL ((uint64_t)0x0000000000000000)
#define B_TRUE ((uint64_t)0x0001800000000000)
#define B_FALSE ((uint64_t)0x0001000000000000)

#define B_I32 ((uint64_t)0x1000000000000000)
#define B_STR_PTR ((uint64_t)0x1000800000000000)
#define B_STRUCT_PTR ((uint64_t)0x1001000000000000)
#define B_VOID_PTR ((uint64_t)0x1001800000000000)

NanBox::NanBox(double x) {
  memcpy(&value, &x, sizeof(double));
  if (value | NAN && value ^ QNAN) { // value is nan and not quiet nan
    value |= QNAN;
  }
}

NanBox::NanBox(int32_t x) {
  memcpy(left_aligned(value, x), &x, sizeof(int32_t));
  value |= B_I32;
}

NanBox::NanBox(bool x) { memcpy(left_aligned(value, x), &x, sizeof(bool)); }

} // namespace exprparse
