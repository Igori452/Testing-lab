#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include "include/gherkin-catch-module.hpp"

// Глобально инициализируем
INIT_GHERKIN("features");

TEST_CASE("Calculator scenario", "[gherkin]") {
    GHERKIN_GIVEN("calculator is running");

    int a = 1;
    int b = 1;

    GHERKIN_WHEN("I add 1 and 1");

    int result = a + b;

    GHERKIN_THEN("result should be 2");

    REQUIRE(result == 2);
}

TEST_CASE("Gherkin coverage check", "[gherkin]") {
    REQUIRE_GHERKIN_COVERAGE();
}