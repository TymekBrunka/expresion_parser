#include <NanBoxing.hpp>
#include <cstring>
namespace exprparse {

#define right_aligned(v, x) &((uint8_t *)&(v))[8 - sizeof((x))]

#define NAN_ ((uint64_t)0x7ffc000000000000) // <62; 50>
#define QNAN ((uint64_t)0x0008000000000000) // 1 << 51
#define IS_NAN_B                                                               \
  ((uint64_t)0x7ff0000000000000) // <62; 50>\{51, 50} bit 50 is intel only and
                                 // bit 51 is quiet nan bit which doesnt
                                 // determine whether it is nan or not

#define TAG_MASK ((uint64_t)0x8003000000000000) // bits 48 and 49
#define IS_BOOL_MASK ((uint64_t)0x8002000000000000)
#define PTR_MASK

#define B_NAN ((uint64_t)0x0001000000000000)
#define B_NULL ((uint64_t)0x0000000000000000)
#define B_TRUE ((uint64_t)0x0003000000000000)
#define B_FALSE ((uint64_t)0x0002000000000000)

#define B_I32 ((uint64_t)0x1000000000000000)
#define B_STR_PTR ((uint64_t)0x1001000000000000)
#define B_STRUCT_PTR ((uint64_t)0x1002000000000000)
#define B_VOID_PTR ((uint64_t)0x1003000000000000)

NanBox::NanBox() {
  value |= NAN_ | B_NULL;
}

NanBox::NanBox(double x) {
  memcpy(&value, &x, sizeof(double));
  if ((value & IS_NAN_B) == IS_NAN_B)
    value |= NAN_ | B_NAN;
}

NanBox::NanBox(int32_t x) {
  memset(&value, 0, sizeof(uint64_t));
  memcpy(right_aligned(value, x), &x, sizeof(int32_t));
  value |= B_I32 | NAN_;
}

NanBox::NanBox(bool x) {
  memset(&value, 0, sizeof(uint64_t));
  value |= (x ? B_TRUE : B_FALSE) | NAN_;
}

NanBox::NanBox(void *x) {
  memcpy(right_aligned(value, x), &x, sizeof(void *));
  value |= B_VOID_PTR | NAN_;
}

NanBox::NanBox(std::string *x) {
  memcpy(right_aligned(value, x), &x, sizeof(std::string *));
  value |= B_STR_PTR | NAN_;
}

NanBox::NanBox(ExprParse_Struct *x) {
  memcpy(right_aligned(value, x), &x, sizeof(ExprParse_Struct *));
  value |= B_STRUCT_PTR | NAN_;
}

nanbox_type NanBox::type() const {
  if ((value & NAN_) != NAN_)
    return nanbox_type::DOUBLE;

  switch (value & TAG_MASK) {

  case B_NAN:
    return nanbox_type::NAN;
  case B_NULL:
    return nanbox_type::NULL_;
  case B_I32:
    return nanbox_type::I32;
  case B_TRUE:
    return nanbox_type::BOOL;
  case B_FALSE:
    return nanbox_type::BOOL;
  case B_VOID_PTR:
    return nanbox_type::VOID_PTR;
  case B_STR_PTR:
    return nanbox_type::STR_PTR;
  case B_STRUCT_PTR:
    return nanbox_type::STRUCT_PTR;
  default:
    return nanbox_type::NAN;
  }
}

const char* NanBox::type_name() const {
  return dbg_type(type());
}

const char *dbg_type(nanbox_type x) {
  switch (x) {
  case nanbox_type::DOUBLE:
    return "double";
  case nanbox_type::NAN:
    return "NaN";
  case nanbox_type::NULL_:
    return "null";
  case nanbox_type::I32:
    return "i32";
  case nanbox_type::BOOL:
    return "bool";
  case nanbox_type::VOID_PTR:
    return "void*";
  case nanbox_type::STR_PTR:
    return "std::string*";
  case nanbox_type::STRUCT_PTR:
    return "ExprParse_Struct*";
  default:
    return "NaN";
  }
}

} // namespace exprparse
