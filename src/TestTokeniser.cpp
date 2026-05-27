#include <ExpresionParser.hpp>
#include <cassert>
#include <cmath> // Required for std::abs
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// Structural test wrapper mapping tags, native Box payload, and string
// expectations
struct Expected {
  exprparse::token_tag tag;
  exprparse::Box box;
  std::string expected_str; // For testing identifiers and string literals

  // Constructors to maintain clean single-line array initializations
  Expected(exprparse::token_tag t)
      : tag(t), box{.is_const = true, ._tag = exprparse::tag::NONE},
        expected_str("") {}

  Expected(exprparse::token_tag t, const std::string &s)
      : tag(t), box{.is_const = true, ._tag = exprparse::tag::NONE},
        expected_str(s) {}

  Expected(exprparse::token_tag t, int i) : tag(t), expected_str("") {
    box.is_const = true;
    box._tag = exprparse::tag::INT;
    box._int = i;
  }

  Expected(exprparse::token_tag t, float f) : tag(t), expected_str("") {
    box.is_const = true;
    box._tag = exprparse::tag::FLOAT;
    box._float = f;
  }
};

// Helper function to safely compare floats and avoid atof/float precision
// mismatches
bool approx_equal(float a, float b, float epsilon = 1e-4f) {
  return std::abs(a - b) < epsilon;
}

void run_test_case(const std::string &name, const std::string &expression,
                   const std::vector<Expected> &expected) {
  exprparse::ExpresionParser parser;
  parser.reader = (char *)expression.data();

  std::vector<exprparse::token> tokens;
  exprparse::token tok;

  // Tokenize until EOS or end of string buffer
  while (tok.tag != exprparse::token_tag::EOS ||
         parser.reader < &expression[expression.size()]) {
    tok = parser.get_token();
    tokens.push_back(tok);
    if (tok.tag == exprparse::token_tag::EOS)
      break;
  }

  // Verify token count (including EOS)
  if (tokens.size() != expected.size()) {
    std::cerr << "[FAIL] " << name << ": Token count mismatch. Expected "
              << expected.size() << ", got " << tokens.size() << "\n";
    assert(tokens.size() == expected.size());
  }

  for (size_t i = 0; i < tokens.size(); ++i) {
    // 1. Assert Token Tag correctness
    if (tokens[i].tag != expected[i].tag) {
      std::cerr << "[FAIL] " << name << " at token [" << i
                << "]: Tag mismatch. Expected "
                << exprparse::tag2str(expected[i].tag) << ", got "
                << exprparse::tag2str(tokens[i].tag) << "\n";
      assert(tokens[i].tag == expected[i].tag);
    }

    // 2. Assert Box Data payload correctness (INT/FLOAT)
    if (expected[i].box._tag != exprparse::tag::NONE) {
      assert(tokens[i].data_idx != -1);
      exprparse::Box actual_box = parser.token_data[tokens[i].data_idx];

      assert(actual_box._tag == expected[i].box._tag);

      if (actual_box._tag == exprparse::tag::INT) {
        if (actual_box._int != expected[i].box._int) {
          std::cerr << "[FAIL] " << name << " at token [" << i
                    << "]: INT data mismatch. Expected " << expected[i].box._int
                    << ", got " << actual_box._int << "\n";
          assert(actual_box._int == expected[i].box._int);
        }
      } else if (actual_box._tag == exprparse::tag::FLOAT) {
        if (!approx_equal(actual_box._float, expected[i].box._float)) {
          std::cerr << "[FAIL] " << name << " at token [" << i
                    << "]: FLOAT data mismatch. Expected "
                    << expected[i].box._float << ", got " << actual_box._float
                    << "\n";
          assert(approx_equal(actual_box._float, expected[i].box._float));
        }
      }
    }

    // 3. Assert String / Identifier Data payload matches expectations
    if (!expected[i].expected_str.empty()) {
      assert(tokens[i].data_idx != -1);
      exprparse::Box actual_box = parser.token_data[tokens[i].data_idx];

      std::stringstream actual_str_stream{};

      // Replicating your original debug string extract logic using your
      // STRUCT_PTR printer
      if (actual_box._tag == exprparse::tag::STRUCT_PTR) {
        actual_str_stream << actual_box.struct_ptr->print();
      }

      std::string actual_str = actual_str_stream.str();
      if (actual_str != expected[i].expected_str) {
        std::cerr << "[FAIL] " << name << " at token [" << i
                  << "]: String payload mismatch.\n"
                  << "  Expected: [" << expected[i].expected_str << "]\n"
                  << "  Got:      [" << actual_str << "]\n";
        assert(actual_str == expected[i].expected_str);
      }
    }
  }
  std::cout << "[PASS] " << name << "\n";
}

int main() {
  std::cout << "Running Tokenizer Test Suite (Strict Box & String Payload "
               "Checking)...\n\n";

  // 1. Your original integration debug string, fully automated with float AND
  // string validation
  run_test_case(
      "Original Integration String",
      "hi . hello ( cat != 12,\t67.89 / 34 *= ++ --  ) "
      "\"Hello\\a\\t\\x1b[32mgeen\\x1b[0m World\\n \\x1b[31mhai\\x1b[0m \"  ",
      {{exprparse::token_tag::IDENTIFIER,
        "hi"}, // Validates string output of STRUCT_PTR maps to "hi"
       {exprparse::token_tag::DOT_ACCESS},
       {exprparse::token_tag::IDENTIFIER, "hello"}, // Validates identifier text
       {exprparse::token_tag::OPEN_PARENT},
       {exprparse::token_tag::IDENTIFIER, "cat"}, // Validates identifier text
       {exprparse::token_tag::NOT_EQUALS},
       {exprparse::token_tag::INT_LIT, 12},
       {exprparse::token_tag::COMMA},
       {exprparse::token_tag::FLOAT_LIT, 67.89f},
       {exprparse::token_tag::SLASH},
       {exprparse::token_tag::INT_LIT, 34},
       {exprparse::token_tag::STAR_EQUALS},
       {exprparse::token_tag::PLUS_PLUS},
       {exprparse::token_tag::MINUS_MINUS},
       {exprparse::token_tag::CLOSE_PARENT},
       // Checks raw multi-line, escaped, colorized string literal match
       {exprparse::token_tag::STRING_LIT,
        "Hello\a\t\x1b[32mgeen\x1b[0m World\n \x1b[31mhai\x1b[0m "},
       {exprparse::token_tag::EOS}});

  // 2. Greedy Operators Lookahead
  run_test_case("Greedy Operators Lookahead",
                "= == + ++ += - -- -= * *= / /= % %=",
                {{exprparse::token_tag::EQUALS},
                 {exprparse::token_tag::EQUALS_EQUALS},
                 {exprparse::token_tag::PLUS},
                 {exprparse::token_tag::PLUS_PLUS},
                 {exprparse::token_tag::PLUS_EQUALS},
                 {exprparse::token_tag::MINUS},
                 {exprparse::token_tag::MINUS_MINUS},
                 {exprparse::token_tag::MINUS_EQUALS},
                 {exprparse::token_tag::STAR},
                 {exprparse::token_tag::STAR_EQUALS},
                 {exprparse::token_tag::SLASH},
                 {exprparse::token_tag::SLASH_EQUALS},
                 {exprparse::token_tag::MOD},
                 {exprparse::token_tag::MOD_EQUALS},
                 {exprparse::token_tag::EOS}});

  // 3. Delimiter Packing
  run_test_case("No-Whitespace Numeric Packing", "12+=34.56--78",
                {{exprparse::token_tag::INT_LIT, 12},
                 {exprparse::token_tag::PLUS_EQUALS},
                 {exprparse::token_tag::FLOAT_LIT, 34.56f},
                 {exprparse::token_tag::MINUS_MINUS},
                 {exprparse::token_tag::INT_LIT, 78},
                 {exprparse::token_tag::EOS}});

  // 4. Edge Case: Strange identifiers with explicit string matching
  run_test_case("Identifier Structure", "var_123 _secret 45 num",
                {{exprparse::token_tag::IDENTIFIER, "var_123"},
                 {exprparse::token_tag::IDENTIFIER, "_secret"},
                 {exprparse::token_tag::INT_LIT, 45},
                 {exprparse::token_tag::IDENTIFIER, "num"},
                 {exprparse::token_tag::EOS}});

  // 4. Edge Case: Malformed Float Representations
  run_test_case("Malformed Float Check", "12.34.56",
                {{exprparse::token_tag::INVALID},
                 {exprparse::token_tag::INT_LIT, 56},
                 {exprparse::token_tag::EOS}});

  // 5. Edge Case: Unclosed String Handling (Guards against buffer overruns at
  // EOF)
  run_test_case("Unclosed String Handling", "\"this string never ends",
                {{exprparse::token_tag::INVALID}, {exprparse::token_tag::EOS}});

  // 6. Boundary Case: Empty input and whitespace only
  run_test_case("Empty / Whitespace Input", "   \t\n  ",
                {{exprparse::token_tag::EOS}});

  std::cout << "\nAll tests completed successfully!\n";
  return 0;
}
