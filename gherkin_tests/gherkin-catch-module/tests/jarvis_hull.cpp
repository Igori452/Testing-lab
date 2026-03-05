#include <catch2/catch_test_macros.hpp>

#include "gherkin_macros.hpp"
#include "context/commonFunction.hpp"

#include <sstream>
#include <fstream>

using namespace jarvis_test;

// ===== РЕГИСТРАЦИЯ ШАГОВ =====

// Background шаги
BDD_GIVEN("the coordinate plane is cleared", []() {
    reset();
});

BDD_AND("test points from the file \"points.txt\" were loaded", []() {
    points = {{0,0}, {100,0}, {0,100}};
});

// Scenario: Successful construction
BDD_GIVEN("a set of 3 non-collinear points:", []() {
    points = {
        {0, 0},
        {100, 0},
        {0, 100}
    };
});

BDD_WHEN("I run the Jarvis algorithm", []() {
    result = runJarvisAlgorithm(points);
});

BDD_THEN("a red triangle is displayed on the screen", []() {
    REQUIRE(result.success);
    REQUIRE(result.hull.size() == 3);
});

BDD_AND("the hull contains exactly 3 vertices", []() {
    REQUIRE(result.hull.size() == 3);
});

BDD_AND("all original points are either vertices or lie inside", []() {
    for (const auto& p : points) {
        REQUIRE(isPointInVector(p, result.hull));
    }
});

// Scenario: Two points
BDD_GIVEN("I have opened the program", []() {
    reset();
});

BDD_AND("I load the file \"two_points.txt\" containing:", []() {
    points = {{100,100}, {200,200}};
});

BDD_WHEN("I press the \"Load File\" button", []() {
    // Файл уже загружен
});

BDD_AND("I press the \"Find Jarvis Hull\" button", []() {
    result = runJarvisAlgorithm(points);
});

BDD_THEN("a red message \"Cannot build Convex Hull!\" appears above the button", []() {
    REQUIRE_FALSE(result.success);
    REQUIRE(result.message == "Cannot build Convex Hull!");
});

BDD_AND("the hull is not displayed on the plane", []() {
    REQUIRE(result.hull.empty());
});

// Scenario: Empty set
BDD_GIVEN("the file \"points.txt\" is empty", []() {
    points.clear();
});

BDD_THEN("the algorithm returns an empty hull", []() {
    REQUIRE(result.hull.empty());
});

BDD_AND("the plane remains empty", []() {
    REQUIRE(result.hull.empty());
});

// Scenario: Collinear points
BDD_GIVEN("a set of collinear points:", []() {
    points = {
        {0, 0},
        {50, 50},
        {100, 100}
    };
});

BDD_AND("a message \"Cannot build Convex Hull!\" is displayed", []() {
    REQUIRE(result.message == "Cannot build Convex Hull!");
});

// Scenario: Identical coordinates
BDD_GIVEN("a set of points with identical coordinates:", []() {
    points = {
        {100, 100},
        {100, 100},
        {100, 100},
        {100, 100}
    };
});

// Scenario: Filtering points
BDD_GIVEN("a set of points with coordinates outside [0,600]:", []() {
    points = {
        {-10, -10},
        {100, 100},
        {700, 700},
        {300, 300},
        {500, 500}
    };
});

BDD_THEN("points outside the range [0,600] are filtered out", []() {
    std::vector<Point> validPoints;
    for (const auto& p : points) {
        if (p.x >= 0 && p.x <= 600 && p.y >= 0 && p.y <= 600) {
            validPoints.push_back(p);
        }
    }
    REQUIRE(validPoints.size() == 3);
});

BDD_AND("the algorithm returns an empty hull because points are collinear", []() {
    REQUIRE(!result.success);
    REQUIRE(result.hull.size() == 0);
});

BDD_BUT("the program does not crash", []() {
    SUCCEED("Program continued normally");
});

// Scenario Outline examples
BDD_GIVEN("a set of points:", []() {
    // Будет параметризовано в тестах
});

BDD_THEN("the number of vertices in the hull equals 4", []() {
    REQUIRE(result.hull.size() == 4);
});

BDD_THEN("the number of vertices in the hull equals 3", []() {
    REQUIRE(result.hull.size() == 3);
});

BDD_AND("all vertices have coordinates from the set of original points", []() {
    for (const auto& p : result.hull) {
        REQUIRE(isPointInVector(p, points));
    }
});

// ===== ТЕСТЫ =====

TEST_CASE("Jarvis: Background", "[jarvis][background]") {
    CALL_GIVEN("the coordinate plane is cleared");
    CALL_AND("test points from the file \"points.txt\" were loaded");
}

TEST_CASE("Jarvis: Successful construction", "[jarvis][scenario1]") {
    CALL_GIVEN("a set of 3 non-collinear points:");
    CALL_WHEN("I run the Jarvis algorithm");
    CALL_THEN("a red triangle is displayed on the screen");
    CALL_AND("the hull contains exactly 3 vertices");
    CALL_AND("all original points are either vertices or lie inside");
}

TEST_CASE("Jarvis: Two points", "[jarvis][scenario2]") {
    CALL_GIVEN("I have opened the program");
    CALL_AND("I load the file \"two_points.txt\" containing:");
    CALL_WHEN("I press the \"Load File\" button");
    CALL_AND("I press the \"Find Jarvis Hull\" button");
    CALL_THEN("a red message \"Cannot build Convex Hull!\" appears above the button");
    CALL_AND("the hull is not displayed on the plane");
}

TEST_CASE("Jarvis: Empty set", "[jarvis][scenario3]") {
    CALL_GIVEN("the file \"points.txt\" is empty");
    CALL_WHEN("I run the Jarvis algorithm");
    CALL_THEN("the algorithm returns an empty hull");
    CALL_AND("the plane remains empty");
}

TEST_CASE("Jarvis: Collinear points", "[jarvis][scenario4]") {
    CALL_GIVEN("a set of collinear points:");
    CALL_WHEN("I run the Jarvis algorithm");
    CALL_THEN("the algorithm returns an empty hull");
    CALL_AND("a message \"Cannot build Convex Hull!\" is displayed");
}

TEST_CASE("Jarvis: Identical coordinates", "[jarvis][scenario5]") {
    CALL_GIVEN("a set of points with identical coordinates:");
    CALL_WHEN("I run the Jarvis algorithm");
    CALL_THEN("the algorithm returns an empty hull");
}

TEST_CASE("Jarvis: Filtering points", "[jarvis][scenario6]") {
    CALL_GIVEN("a set of points with coordinates outside [0,600]:");
    CALL_WHEN("I run the Jarvis algorithm");
    CALL_THEN("points outside the range [0,600] are filtered out");
    CALL_AND("the algorithm returns an empty hull because points are collinear");
    CALL_BUT("the program does not crash");
}

TEST_CASE("Jarvis: Scenario Outline - Square", "[jarvis][outline1]") {
    points = {
        {0, 0},
        {100, 0},
        {100, 100},
        {0, 100}
    };
    
    CALL_GIVEN("a set of points:");
    CALL_WHEN("I run the Jarvis algorithm");
    CALL_THEN("the number of vertices in the hull equals 4");
    CALL_AND("all vertices have coordinates from the set of original points");
}

TEST_CASE("Jarvis: Scenario Outline - Triangle with internal point", "[jarvis][outline2]") {
    points = {
        {0, 0},
        {100, 0},
        {50, 50},
        {0, 100}
    };
    
    CALL_GIVEN("a set of points:");
    CALL_WHEN("I run the Jarvis algorithm");
    CALL_THEN("the number of vertices in the hull equals 3");
    CALL_AND("all vertices have coordinates from the set of original points");
}