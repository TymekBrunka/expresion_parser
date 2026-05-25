#include <ExpresionParser.hpp>
#include <iostream>
#include <cassert>
#include <vector>

// Struktura pomocnicza do definiowania oczekiwanego wyniku testu
struct ExpectedToken {
    exprparse::token_tag tag;
    std::string expected_value_str; // Opcjonalnie do sprawdzenia zawartości
};

// Uniwersalna funkcja uruchamiająca parser dla danego tekstu
std::vector<exprparse::token> run_lexer(const std::string& expr, exprparse::ExpresionParser& parser) {
    parser.reader = (char*)expr.data();
    exprparse::token tok;
    std::vector<exprparse::token> tokens{};

    while (tok.tag != exprparse::token_tag::EOS ||
           parser.reader < &expr[expr.size()]) {
        tok = parser.get_token();
        tokens.push_back(tok);
    }
    return tokens;
}

// TEST 1: Sprawdzenie podstawowych identyfikatorów i operatorów
void test_basic_tokens() {
    exprparse::ExpresionParser parser;
    // Testujemy: słowo, kropka, słowo, nawias
    std::string expr = "hi . hello ("; 
    
    auto tokens = run_lexer(expr, parser);

    // Oczekujemy 4 tokenów przed końcem strumienia (lub z EOS, zależnie od implementacji)
    assert(tokens.size() >= 4);
    
    // UWAGA: Dostosuj poniższe nazwy tagów (np. IDENTIFIER, DOT_ACCESS) do Twojego pliku ExpresionParser.hpp
    assert(tokens[0].tag == exprparse::token_tag::IDENTIFIER);
    assert(tokens[1].tag == exprparse::token_tag::DOT_ACCESS);
    assert(tokens[2].tag == exprparse::token_tag::IDENTIFIER);
    assert(tokens[3].tag == exprparse::token_tag::OPEN_PARENT);
    
    std::cout << "[OK] Test podstawowych tokenow zaliczony.\n";
}

// TEST 2: Sprawdzenie poprawności parsowania liczb (INT oraz FLOAT)
void test_numbers_and_data() {
    exprparse::ExpresionParser parser;
    std::string expr = "12 67.89 34";
    
    auto tokens = run_lexer(expr, parser);
    
    assert(tokens.size() >= 3);

    // Sprawdzenie int: 12
    assert(tokens[0].data_idx != -1);
    auto data0 = parser.token_data[tokens[0].data_idx];
    assert(data0._tag == exprparse::tag::INT);
    assert(data0._int == 12);

    // Sprawdzenie float: 67.89
    assert(tokens[1].data_idx != -1);
    auto data1 = parser.token_data[tokens[1].data_idx];
    assert(data1._tag == exprparse::tag::FLOAT);
    // Przy float bezpieczniej sprawdzać bliskość z uwagi na precyzję, lub bezpośrednio:
    assert(data1._float > 67.88 && data1._float < 67.90);

    // Sprawdzenie int: 34
    assert(tokens[2].data_idx != -1);
    auto data2 = parser.token_data[tokens[2].data_idx];
    assert(data2._tag == exprparse::tag::INT);
    assert(data2._int == 34);

    std::cout << "[OK] Test parsowania liczb (INT/FLOAT) zaliczony.\n";
}

// TEST 3: Sprawdzenie odporności na białe znaki (spacje, tabulatory)
void test_whitespace_handling() {
    exprparse::ExpresionParser parser;
    // Dużo spacji i tabulacji między dwoma operatorami
    std::string expr = "cat \t\t  !=   \t 12";
    
    auto tokens = run_lexer(expr, parser);
    
    // Powinny wyekstrahować się dokładnie 3 tokeny (identyfikator, operator, liczba)
    // Ignorując całkowicie białe znaki
    assert(tokens.size() >= 3);
    assert(tokens[0].tag == exprparse::token_tag::IDENTIFIER);
    assert(tokens[1].tag == exprparse::token_tag::NOT_EQUALS); // Przykład nazwy tagu dla !=
    
    std::cout << "[OK] Test ignorowania bialych znakow zaliczony.\n";
}

// TEST 4: Sprawdzenie pustego stringa i samego końca strumienia (Edge Case)
void test_empty_expression() {
    exprparse::ExpresionParser parser;
    std::string expr = "";
    
    auto tokens = run_lexer(expr, parser);
    
    // Jeśli string jest pusty, lexer powinien od razu zwrócić EOS
    assert(!tokens.empty());
    assert(tokens[0].tag == exprparse::token_tag::EOS);
    
    std::cout << "[OK] Test pustego wyrazenia zaliczony.\n";
}

int main() {
    std::cout << "--- URUCHAMIANIE TESTOW JEDNOSTKOWYCH LEXERA ---\n";
    
    test_basic_tokens();
    test_numbers_and_data();
    test_whitespace_handling();
    test_empty_expression();
    
    std::cout << "------------------------------------------------\n";
    std::cout << "WSZYSTKIE TESTY ZAKONCZONE SUKCESEM!\n";
    return 0;
}
