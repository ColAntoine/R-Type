#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "string.h"

int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}

TEST_CASE("testing the factorial function") {
    CHECK(factorial(1) == 1);
    CHECK(factorial(2) == 2);
    CHECK(factorial(3) == 6);
    CHECK(factorial(10) == 3628800);
}

std::string addReturnToLine(std::string str) {
    return str + "\n";
}

TEST_CASE("test the Return to line function") {
    CHECK(addReturnToLine("hello") == "hello\n");
}