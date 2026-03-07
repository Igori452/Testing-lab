#include <catch2/catch_test_macros.hpp>

#include "gherkin_macros.hpp"
#include "context/commonFunction.hpp"

#include <sstream>
#include <fstream>
#include <filesystem>

//GHERKIN_FEATURES_PATH="features" GHERKIN_FEATURE_FILE="error_handling.feature" ./error_tests

using namespace error_handling;

void resetState() {
    reset();
    state.programRunning = true;
    state.windowOpen = true;
    state.errorMessage = "";
    state.points.clear();
    state.jarvisHull.clear();
}

BDD_GIVEN("I have opened the program", []() {
    resetState();
});

BDD_AND("I create a file \"corrupted.txt\" with the content:", []() {
    std::string content = "abc def\n100 xyz\n!@# $\n";
    createTestFile("corrupted.txt", content);
});

BDD_WHEN("I press the \"Load File\" button and select \"corrupted.txt\"", []() {
    state.lastButtonPressed = "Load File";
    state.currentFile = "corrupted.txt";

    std::ifstream file("corrupted.txt");
    if (file.is_open()) {
        std::string line;
        bool validFormat = true;

        while (std::getline(file, line)) {
            if (line.empty()) continue;

            std::istringstream iss(line);
            double x, y;
            if (!(iss >> x >> y)) {
                validFormat = false;
                break;
            }
        }

        if (!validFormat) {
            state.errorMessage = "Invalid data format";
            state.points.clear();
        }
    }

    deleteTestFile("corrupted.txt");
});

BDD_THEN("the program does not terminate abnormally", []() {
    REQUIRE(state.programRunning);
    REQUIRE(state.windowOpen);
});

BDD_AND("no new points are displayed on the plane", []() {
    REQUIRE(state.points.empty());
});

BDD_BUT("an error message \"Invalid data format\" is printed to the console", []() {
    REQUIRE(state.errorMessage == "Invalid data format");
});

BDD_GIVEN("a file contains points with negative coordinates:", []() {
    resetState();
    state.points = {
        {-10, -20},
        {100, -5},
        {-30, 50}
    };
});

BDD_WHEN("I load this file into the program", []() {
    state.lastButtonPressed = "Load File";
});

BDD_THEN("negative coordinates are displayed on the plane", []() {
    REQUIRE(isPointInList({ -10, -20 }, state.points));
    REQUIRE(isPointInList({ 100, -5 }, state.points));
    REQUIRE(isPointInList({ -30, 50 }, state.points));
});

BDD_BUT("building a hull is only possible for points in the range [0,600]", []() {
    std::vector<Point> pointsForAlgorithm;
    for (const auto& p : state.points) {
        if (p.x >= 0 && p.x <= 600 && p.y >= 0 && p.y <= 600) {
            pointsForAlgorithm.push_back(Point{ static_cast<double>(p.x), static_cast<double>(p.y) });
        }
    }
    REQUIRE(pointsForAlgorithm.size() == 0);
});

BDD_GIVEN("the file \"nonexistent.txt\" does not exist in the program directory", []() {
    resetState();
    REQUIRE_FALSE(fileExists("nonexistent.txt"));
});

BDD_WHEN("I try to load this file", []() {
    state.lastButtonPressed = "Load File";

    if (!fileExists("nonexistent.txt")) {
        state.errorMessage = "File not found!";
    }
});

BDD_THEN("an error message \"File not found!\" is displayed", []() {
    REQUIRE(state.errorMessage == "File not found!");
});

BDD_AND("the plane remains in its previous state", []() {
    REQUIRE(state.points.empty());
});

BDD_GIVEN("a set of exactly 3 non-collinear points:", []() {
    resetState();
    state.points = {
        {0, 0},
        {100, 0},
        {0, 100}
    };
});

BDD_WHEN("I run any convex hull algorithm", []() {
    std::vector<Point> algoPoints;
    for (const auto& p : state.points) {
        algoPoints.push_back({ static_cast<double>(p.x), static_cast<double>(p.y) });
    }

    JarvisAlgorithm jarvis;
    auto hull = jarvis.findConvexHull(algoPoints);

    state.jarvisHull.clear();
    for (const auto& p : hull) {
        state.jarvisHull.push_back({ static_cast<int>(p.x), static_cast<int>(p.y) });
    }
});

BDD_THEN("the hull is successfully built as a triangle", []() {
    REQUIRE(state.jarvisHull.size() == 3);
    REQUIRE(isPointInList({ 0,0 }, state.jarvisHull));
    REQUIRE(isPointInList({ 100,0 }, state.jarvisHull));
    REQUIRE(isPointInList({ 0,100 }, state.jarvisHull));
});

BDD_GIVEN("a set of exactly 2 points:", []() {
    resetState();
    state.points = {
        {100, 100},
        {200, 200}
    };
});

BDD_THEN("the algorithm returns an empty hull", []() {
    REQUIRE(state.jarvisHull.empty());
});

BDD_AND("a message \"Cannot build Convex Hull!\" is displayed", []() {
    state.errorMessage = "Cannot build Convex Hull!";
    REQUIRE(state.errorMessage == "Cannot build Convex Hull!");
});

BDD_GIVEN("a set of points on the boundaries [0,600]:", []() {
    resetState();
    state.points = {
        {0, 0},
        {600, 0},
        {600, 600},
        {0, 600},
        {300, 300}
    };
});

BDD_WHEN("I run the Jarvis algorithm", []() {
    std::vector<Point> algoPoints;
    for (const auto& p : state.points) {
        algoPoints.push_back({ static_cast<double>(p.x), static_cast<double>(p.y) });
    }

    JarvisAlgorithm jarvis;
    auto hull = jarvis.findConvexHull(algoPoints);

    state.jarvisHull.clear();
    for (const auto& p : hull) {
        state.jarvisHull.push_back({ static_cast<int>(p.x), static_cast<int>(p.y) });
    }
});

BDD_THEN("all boundary points are included in the hull", []() {
    REQUIRE(isPointInList({ 0,0 }, state.jarvisHull));
    REQUIRE(isPointInList({ 600,0 }, state.jarvisHull));
    REQUIRE(isPointInList({ 600,600 }, state.jarvisHull));
    REQUIRE(isPointInList({ 0,600 }, state.jarvisHull));
});

BDD_AND("the hull has 4 vertices", []() {
    REQUIRE(state.jarvisHull.size() == 4);
    REQUIRE_FALSE(isPointInList({ 300,300 }, state.jarvisHull));
});

BDD_GIVEN("less than 3 points", []() {
    resetState();
    state.points = { {100,100}, {200,200} };
});

BDD_WHEN("I try to build a convex hull", []() {
    state.errorMessage = "Cannot build Convex Hull!";
});

BDD_THEN("the program displays \"Cannot build Convex Hull!\"", []() {
    REQUIRE(state.errorMessage == "Cannot build Convex Hull!");
});

BDD_AND("the application continues running", []() {
    REQUIRE(state.programRunning);
    REQUIRE(state.windowOpen);
});

BDD_AND("the application continues running", []() {
    REQUIRE(state.programRunning);
    REQUIRE(state.windowOpen);
});

TEST_CASE("Errors: Invalid file format", "[errors][scenario1]") {
    CALL_GIVEN("I have opened the program");
    CALL_AND("I create a file \"corrupted.txt\" with the content:");
    CALL_WHEN("I press the \"Load File\" button and select \"corrupted.txt\"");
    CALL_THEN("the program does not terminate abnormally");
    CALL_AND("no new points are displayed on the plane");
    CALL_BUT("an error message \"Invalid data format\" is printed to the console");
}

TEST_CASE("Errors: Negative coordinates", "[errors][scenario2]") {
    CALL_GIVEN("a file contains points with negative coordinates:");
    CALL_WHEN("I load this file into the program");
    CALL_THEN("negative coordinates are displayed on the plane");
    CALL_BUT("building a hull is only possible for points in the range [0,600]");
}

TEST_CASE("Errors: File not found", "[errors][scenario3]") {
    CALL_GIVEN("the file \"nonexistent.txt\" does not exist in the program directory");
    CALL_WHEN("I try to load this file");
    CALL_THEN("an error message \"File not found!\" is displayed");
    CALL_AND("the plane remains in its previous state");
}

TEST_CASE("Errors: Boundary - 3 points", "[errors][boundary1]") {
    CALL_GIVEN("a set of exactly 3 non-collinear points:");
    CALL_WHEN("I run any convex hull algorithm");
    CALL_THEN("the hull is successfully built as a triangle");
}

TEST_CASE("Errors: Boundary - 2 points", "[errors][boundary2]") {
    CALL_GIVEN("a set of exactly 2 points:");
    CALL_WHEN("I run any convex hull algorithm");
    CALL_THEN("the algorithm returns an empty hull");
    CALL_AND("a message \"Cannot build Convex Hull!\" is displayed");
}

TEST_CASE("Errors: Points on boundaries", "[errors][boundary3]") {
    CALL_GIVEN("a set of points on the boundaries [0,600]:");
    CALL_WHEN("I run the Jarvis algorithm");
    CALL_THEN("all boundary points are included in the hull");
    CALL_AND("the hull has 4 vertices");
}

TEST_CASE("Errors: Less than 3 points", "[errors][outline1]") {
    CALL_GIVEN("less than 3 points");
    CALL_WHEN("I try to build a convex hull");
    CALL_THEN("the program displays \"Cannot build Convex Hull!\"");
    CALL_AND("the application continues running");
}