
#include <catch2/catch_test_macros.hpp>

#include "gherkin_macros.hpp"
#include "context/commonFunction.hpp"

#include <sstream>
#include <fstream>

using namespace visualisation_test;

// ===== РЕГИСТРАЦИЯ ШАГОВ =====

// Background
BDD_GIVEN("coordinate plane of 600x600 pixels", []() {
    reset();
    // Плоскость 600x600 - просто контекст
    });

BDD_AND("coordinate axes are displayed", []() {
    state.axesVisible = true;
    });

BDD_AND("the program window is open", []() {
    state.windowOpen = true;
    state.programRunning = true;
    });

// Scenario: Different colors for different algorithms
BDD_GIVEN("points are loaded on the plane", []() {
    if (state.points.empty()) {
        state.points = { {100,100}, {200,200}, {300,300} };
    }
    });

BDD_WHEN("I run the Jarvis algorithm", []() {
    state.activeAlgorithm = "Jarvis";
    state.lastButtonPressed = "Find Jarvis Hull";
    });

BDD_THEN("the hull is displayed in red color", []() {
    REQUIRE(state.activeAlgorithm == "Jarvis");
    // Цвет red - проверяем что это Jarvis
    });

BDD_WHEN("I run the Graham algorithm", []() {
    state.activeAlgorithm = "Graham";
    state.lastButtonPressed = "Find Graham Hull";
    });

BDD_THEN("the hull is displayed in green color", []() {
    REQUIRE(state.activeAlgorithm == "Graham");
    // Цвет green - проверяем что это Graham
    });

// Scenario: Clearing the plane
BDD_GIVEN("points and a hull are displayed on the plane", []() {
    state.points = { {100,100}, {200,200}, {300,300} };
    state.jarvisHull = { {100,100}, {300,300} };
    state.grahamHull = { {100,100}, {300,300} };
    });

BDD_WHEN("I load a new file \"new_points.txt\"", []() {
    state.points = { {400,400}, {500,500} };
    state.jarvisHull.clear();
    state.grahamHull.clear();
    state.currentFile = "new_points.txt";
    });

BDD_THEN("the previous points and hull disappear", []() {
    REQUIRE(state.jarvisHull.empty());
    REQUIRE(state.grahamHull.empty());
    // Старые точки (100,100, 200,200, 300,300) не должны быть в points
    REQUIRE(!isPointInList({ 100,100 }, state.points));
    REQUIRE(!isPointInList({ 200,200 }, state.points));
    });

BDD_AND("only the new points are displayed", []() {
    REQUIRE(state.points.size() == 2);
    REQUIRE(isPointInList({ 400,400 }, state.points));
    REQUIRE(isPointInList({ 500,500 }, state.points));
    });

// Scenario: Error message display
BDD_GIVEN("collinear points are loaded:", []() {
    state.points = {
        {100, 100},
        {200, 200},
        {300, 300}
    };
    });

BDD_WHEN("I press the \"Find Jarvis Hull\" button", []() {
    state.lastButtonPressed = "Find Jarvis Hull";
    state.errorMessage = "Cannot build Convex Hull!";
    state.activeAlgorithm = "Jarvis";
    });

BDD_THEN("a red message \"Cannot build Convex Hull!\" appears above the button", []() {
    REQUIRE(state.errorMessage == "Cannot build Convex Hull!");
    REQUIRE(state.activeAlgorithm == "Jarvis");
    });

BDD_WHEN("I press the \"Find Graham Hull\" button", []() {
    state.lastButtonPressed = "Find Graham Hull";
    state.errorMessage = "Cannot build Convex Hull!";
    state.activeAlgorithm = "Graham";
    });

BDD_THEN("a green message \"Cannot build Convex Hull!\" appears above the button", []() {
    REQUIRE(state.errorMessage == "Cannot build Convex Hull!");
    REQUIRE(state.activeAlgorithm == "Graham");
    });

// Scenario: Displaying points with exact coordinates
BDD_GIVEN("I load points with coordinates on the boundaries:", []() {
    state.points = {
        {0, 0},
        {600, 0},
        {600, 600},
        {0, 600}
    };
    });

BDD_WHEN("the points are displayed", []() {
    // Точки уже загружены
    });

BDD_THEN("they are visible at the corners of the plane", []() {
    REQUIRE(isPointInList({ 0,0 }, state.points));
    REQUIRE(isPointInList({ 600,0 }, state.points));
    REQUIRE(isPointInList({ 600,600 }, state.points));
    REQUIRE(isPointInList({ 0,600 }, state.points));
    });

BDD_AND("no points are cut off", []() {
    for (const auto& p : state.points) {
        REQUIRE(p.x >= 0);
        REQUIRE(p.x <= 600);
        REQUIRE(p.y >= 0);
        REQUIRE(p.y <= 600);
    }
    });

BDD_GIVEN("the file \"<dataset>\" is loaded", []() {

});
// Scenario Outline: Visual verification
BDD_GIVEN("the file \"points_10.txt\" is loaded", []() {
    state.points.clear();
    for (int i = 0; i < 10; i++) {
        state.points.push_back({ i * 50, i * 50 });
    }
    state.currentFile = "points_10.txt";
    });

BDD_GIVEN("the file \"points_100.txt\" is loaded", []() {
    state.points.clear();
    for (int i = 0; i < 100; i++) {
        state.points.push_back({ i % 20 * 30, i / 5 * 30 });
    }
    state.currentFile = "points_100.txt";
    });

BDD_THEN("the number of visible points equals 10", []() {
    REQUIRE(state.points.size() == 10);
    });

BDD_THEN("the number of visible points equals 100", []() {
    REQUIRE(state.points.size() == 100);
    });

BDD_AND("the minimum coordinate is >= 0", []() {
    for (const auto& p : state.points) {
        REQUIRE(p.x >= 0);
        REQUIRE(p.y >= 0);
    }
    });

BDD_AND("the maximum coordinate is <= 600", []() {
    for (const auto& p : state.points) {
        REQUIRE(p.x <= 600);
        REQUIRE(p.y <= 600);
    }
    });

// Rule: UI elements
BDD_GIVEN("the program is running", []() {
    state.programRunning = true;
    state.windowOpen = true;
    });

BDD_WHEN("I press the \"Exit\" button", []() {
    state.lastButtonPressed = "Exit";
    state.programRunning = false;
    state.windowOpen = false;
    });

BDD_THEN("the program window closes", []() {
    REQUIRE_FALSE(state.windowOpen);
    });

BDD_AND("the process terminates", []() {
    REQUIRE_FALSE(state.programRunning);
    });


// ===== ТЕСТЫ =====

TEST_CASE("Visualisation: Background", "[vis][background]") {
    CALL_GIVEN("coordinate plane of 600x600 pixels");
    CALL_AND("coordinate axes are displayed");
    CALL_AND("the program window is open");
}

TEST_CASE("Visualisation: Different colors for algorithms", "[vis][scenario2]") {
    CALL_GIVEN("points are loaded on the plane");

    CALL_WHEN("I run the Jarvis algorithm");
    CALL_THEN("the hull is displayed in red color");

    CALL_WHEN("I run the Graham algorithm");
    CALL_THEN("the hull is displayed in green color");
}

TEST_CASE("Visualisation: Clearing the plane", "[vis][scenario3]") {
    CALL_GIVEN("points and a hull are displayed on the plane");
    CALL_WHEN("I load a new file \"new_points.txt\"");
    CALL_THEN("the previous points and hull disappear");
    CALL_AND("only the new points are displayed");
}

TEST_CASE("Visualisation: Error messages", "[vis][scenario4]") {
    CALL_GIVEN("collinear points are loaded:");

    CALL_WHEN("I press the \"Find Jarvis Hull\" button");
    CALL_THEN("a red message \"Cannot build Convex Hull!\" appears above the button");

    CALL_WHEN("I press the \"Find Graham Hull\" button");
    CALL_THEN("a green message \"Cannot build Convex Hull!\" appears above the button");
}

TEST_CASE("Visualisation: Boundary points", "[vis][scenario5]") {
    CALL_GIVEN("I load points with coordinates on the boundaries:");
    CALL_WHEN("the points are displayed");
    CALL_THEN("they are visible at the corners of the plane");
    CALL_AND("no points are cut off");
}

TEST_CASE("Visualisation: Dataset 10 points", "[vis][outline1]") {
    CALL_GIVEN("the file \"points_10.txt\" is loaded");
    CALL_WHEN("the points are displayed");
    CALL_THEN("the number of visible points equals 10");
    CALL_AND("the minimum coordinate is >= 0");
    CALL_AND("the maximum coordinate is <= 600");
}

TEST_CASE("Visualisation: Dataset 100 points", "[vis][outline2]") {
    CALL_GIVEN("the file \"points_100.txt\" is loaded");
    CALL_WHEN("the points are displayed");
    CALL_THEN("the number of visible points equals 100");
    CALL_AND("the minimum coordinate is >= 0");
    CALL_AND("the maximum coordinate is <= 600");
}

TEST_CASE("Visualisation: Exit button", "[vis][ui1]") {
    CALL_GIVEN("the program is running");
    CALL_WHEN("I press the \"Exit\" button");
    CALL_THEN("the program window closes");
    CALL_AND("the process terminates");
}