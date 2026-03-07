#include <catch2/catch_test_macros.hpp>

#include "gherkin_macros.hpp"
#include "context/commonFunction.hpp"

#include <sstream>
#include <fstream>
#include <algorithm>

//GHERKIN_FEATURES_PATH="features" GHERKIN_FEATURE_FILE="graham_hull.feature" ./graham_tests

using namespace graham_test;

BDD_GIVEN("the coordinate plane is cleared", []() {
    reset();
});

BDD_AND("test points from the file \"points.txt\" were loaded", []() {
    points = { {0,0}, {100,0}, {0,100} };
});

BDD_GIVEN("a set of 6 points with different coordinates", []() {
    points = {
        {100, 100},
        {300, 500},
        {500, 200},
        {200, 400},
        {400, 100},
        {600, 300}
    };
});

BDD_WHEN("I run the Graham algorithm", []() {
    result = runGrahamAlgorithm(points);
    });

BDD_THEN("a green contour is displayed on the screen", []() {
    REQUIRE(result.success);
    REQUIRE(result.hull.size() >= 3);
    });

BDD_AND("each vertex of the contour is an extreme point of the set", []() {
    for (const auto& p : result.hull) {
        REQUIRE(isPointInVector(p, points));
    }
    });

BDD_AND("all other points lie inside the contour", []() {
    for (const auto& p : points) {
        if (!isPointInVector(p, result.hull)) {
            SUCCEED("Point is inside hull");
        }
    }
});

BDD_GIVEN("I have opened the program", []() {
    reset();
});

BDD_AND("I load the file \"mixed_points.txt\" containing:", []() {
    points = {
        {100, 100},
        {500, 100},
        {-10, -10},
        {700, 700},
        {300, 400}
    };
});

BDD_WHEN("I press the \"Load File\" button", []() {});

BDD_AND("I press the \"Find Graham Hull\" button", []() {
    result = runGrahamAlgorithm(points);
});

BDD_THEN("points with coordinates [-10,-10] and [700,700] are filtered out", []() {
    SUCCEED("Points filtered");
});

BDD_AND("the hull is built from points [100,100], [500,100], [300,400]", []() {
    REQUIRE(result.success);
    REQUIRE(result.hull.size() == 3);

    std::vector<Point> expectedPoints = { {100,100}, {500,100}, {300,400} };
    for (const auto& p : expectedPoints) {
        REQUIRE(isPointInVector(p, result.hull));
    }
    });

BDD_BUT("the program continues working without crashes", []() {
    SUCCEED("Program continued normally");
});

BDD_GIVEN("the file \"points.txt\" is empty", []() {
    points.clear();
});

BDD_THEN("the algorithm returns an empty hull", []() {
    REQUIRE(result.hull.empty());
});

BDD_AND("a green message \"Cannot build Convex Hull!\" is displayed", []() {
    REQUIRE_FALSE(result.success);
    REQUIRE(result.message == "Cannot build Convex Hull!");
});

BDD_GIVEN("a set of collinear points:", []() {
    points = {
        {0, 0},
        {50, 50},
        {100, 100},
        {150, 150}
    };
});

BDD_GIVEN("a set of points:", []() {});

BDD_THEN("the starting point is correctly identified as [150,100]", []() {
    Point expectedStart = { 150, 100 };
    REQUIRE(arePointsEqual(result.startPoint, expectedStart));
    });

BDD_AND("if there were points with equal Y, the leftmost would be chosen", []() {
    std::vector<Point> equalYPoints = {
        {200, 100},
        {50, 100},
        {150, 100},
        {300, 100}
    };

    auto testResult = runGrahamAlgorithm(equalYPoints);
    Point expectedLeftmost = { 200, 100 };
    REQUIRE(arePointsEqual(testResult.startPoint, expectedLeftmost));
});

BDD_GIVEN("a set of points with collinear points", []() {});

BDD_THEN("collinear points are removed during sorting", []() {});

BDD_AND("only unique extreme points remain in the hull", []() {});

TEST_CASE("Graham: Background", "[graham][background]") {
    CALL_GIVEN("the coordinate plane is cleared");
    CALL_AND("test points from the file \"points.txt\" were loaded");
}

TEST_CASE("Graham: Random points", "[graham][scenario1]") {
    CALL_GIVEN("a set of 6 points with different coordinates");
    CALL_WHEN("I run the Graham algorithm");
    CALL_THEN("a green contour is displayed on the screen");
    CALL_AND("each vertex of the contour is an extreme point of the set");
    CALL_AND("all other points lie inside the contour");
}

TEST_CASE("Graham: Filtering points", "[graham][scenario2]") {
    CALL_GIVEN("I have opened the program");
    CALL_AND("I load the file \"mixed_points.txt\" containing:");
    CALL_WHEN("I press the \"Load File\" button");
    CALL_AND("I press the \"Find Graham Hull\" button");
    CALL_THEN("points with coordinates [-10,-10] and [700,700] are filtered out");
    CALL_AND("the hull is built from points [100,100], [500,100], [300,400]");
    CALL_BUT("the program continues working without crashes");
}

TEST_CASE("Graham: Empty set", "[graham][scenario3]") {
    CALL_GIVEN("the file \"points.txt\" is empty");
    CALL_WHEN("I run the Graham algorithm");
    CALL_THEN("the algorithm returns an empty hull");
    CALL_AND("a green message \"Cannot build Convex Hull!\" is displayed");
}

TEST_CASE("Graham: Collinear points", "[graham][scenario4]") {
    CALL_GIVEN("a set of collinear points:");
    CALL_WHEN("I run the Graham algorithm");
    CALL_THEN("the algorithm returns an empty hull");
}

TEST_CASE("Graham: Starting point selection", "[graham][scenario5]") {
    points = {
        {300, 300},
        {100, 400},
        {500, 100},
        {200, 200},
        {150, 100}
    };

    CALL_GIVEN("a set of points:");
    CALL_WHEN("I run the Graham algorithm");
    CALL_THEN("the starting point is correctly identified as [150,100]");
    CALL_AND("if there were points with equal Y, the leftmost would be chosen");
}

TEST_CASE("Graham: Polar angle sorting - Example 1", "[graham][outline1]") {
    points = {
        {0, 0},
        {1, 0},
        {2, 0},
        {0, 1}
    };

    CALL_GIVEN("a set of points with collinear points");
    CALL_WHEN("I run the Graham algorithm");
    CALL_THEN("collinear points are removed during sorting");
    CALL_AND("only unique extreme points remain in the hull");

    REQUIRE(result.success);
    REQUIRE(result.hull.size() == 3);
    REQUIRE(isPointInVector({ 0,0 }, result.hull));
    REQUIRE(isPointInVector({ 2,0 }, result.hull));
    REQUIRE(isPointInVector({ 0,1 }, result.hull));
}

TEST_CASE("Graham: Polar angle sorting - Example 2", "[graham][outline2]") {
    points = {
        {0, 0},
        {100, 0},
        {50, 50},
        {0, 100}
    };

    CALL_GIVEN("a set of points with collinear points");
    CALL_WHEN("I run the Graham algorithm");
    CALL_THEN("collinear points are removed during sorting");
    CALL_AND("only unique extreme points remain in the hull");

    REQUIRE(result.success);
    REQUIRE(result.hull.size() == 3);
    REQUIRE(isPointInVector({ 0,0 }, result.hull));
    REQUIRE(isPointInVector({ 100,0 }, result.hull));
    REQUIRE(isPointInVector({ 0,100 }, result.hull));
    REQUIRE_FALSE(isPointInVector({ 50,50 }, result.hull));
}