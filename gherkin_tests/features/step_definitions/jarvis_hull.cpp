#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>
#include <vector>
#include <algorithm>

// Структура точки (из твоего проекта)
struct Point {
    double x, y;
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

// Заглушка для алгоритма (замени на реальный)
class JarvisAlgorithm {
public:
    std::vector<Point> findConvexHull(const std::vector<Point>& points) {
        if (points.size() < 3) return {};
        // Проверка на коллинеарность
        if (points.size() == 3) {
            // Проверка что точки не коллинеарны
            double cross = (points[1].y - points[0].y) * (points[2].x - points[1].x) -
                          (points[1].x - points[0].x) * (points[2].y - points[1].y);
            if (std::abs(cross) < 1e-8) return {}; // коллинеарны
        }
        return points; // заглушка
    }
};

// -----------------------------------------------------------------------------
// SCENARIO: Успешное построение для минимального набора точек
// -----------------------------------------------------------------------------
SCENARIO("Jarvis algorithm: successful construction for minimum set", "[jarvis][declarative]") {
    GIVEN("a set of 3 non-collinear points") {
        std::vector<Point> points = {{0, 0}, {100, 0}, {0, 100}};
        JarvisAlgorithm jarvis;
        
        WHEN("I run the Jarvis algorithm") {
            auto hull = jarvis.findConvexHull(points);
            
            THEN("the hull contains exactly 3 vertices") {
                REQUIRE(hull.size() == 3);
            }
            
            THEN("all original points are vertices") {
                CHECK(std::find(hull.begin(), hull.end(), Point{0,0}) != hull.end());
                CHECK(std::find(hull.begin(), hull.end(), Point{100,0}) != hull.end());
                CHECK(std::find(hull.begin(), hull.end(), Point{0,100}) != hull.end());
            }
        }
    }
}

// -----------------------------------------------------------------------------
// SCENARIO: Попытка построить оболочку для двух точек
// -----------------------------------------------------------------------------
SCENARIO("Jarvis algorithm: attempt to build for two points", "[jarvis][imperative]") {
    GIVEN("I have only two points") {
        std::vector<Point> points = {{100, 100}, {200, 200}};
        JarvisAlgorithm jarvis;
        
        WHEN("I run the Jarvis algorithm") {
            auto hull = jarvis.findConvexHull(points);
            
            THEN("the algorithm returns an empty hull") {
                REQUIRE(hull.empty());
            }
        }
    }
}

// -----------------------------------------------------------------------------
// SCENARIO: Пустое множество точек
// -----------------------------------------------------------------------------
SCENARIO("Jarvis algorithm: empty set of points", "[jarvis]") {
    GIVEN("an empty set of points") {
        std::vector<Point> points;
        JarvisAlgorithm jarvis;
        
        WHEN("I run the Jarvis algorithm") {
            auto hull = jarvis.findConvexHull(points);
            
            THEN("the algorithm returns an empty hull") {
                REQUIRE(hull.empty());
            }
        }
    }
}

// -----------------------------------------------------------------------------
// SCENARIO: Все точки коллинеарны
// -----------------------------------------------------------------------------
SCENARIO("Jarvis algorithm: all points are collinear", "[jarvis]") {
    GIVEN("a set of collinear points") {
        std::vector<Point> points = {{0, 0}, {50, 50}, {100, 100}};
        JarvisAlgorithm jarvis;
        
        WHEN("I run the Jarvis algorithm") {
            auto hull = jarvis.findConvexHull(points);
            
            THEN("the algorithm returns an empty hull") {
                REQUIRE(hull.empty());
            }
        }
    }
}

// -----------------------------------------------------------------------------
// SCENARIO: Все точки имеют одинаковые координаты
// -----------------------------------------------------------------------------
SCENARIO("Jarvis algorithm: all points have same coordinates", "[jarvis]") {
    GIVEN("a set of points with identical coordinates") {
        std::vector<Point> points = {{100, 100}, {100, 100}, {100, 100}};
        JarvisAlgorithm jarvis;
        
        WHEN("I run the Jarvis algorithm") {
            auto hull = jarvis.findConvexHull(points);
            
            THEN("the algorithm returns an empty hull") {
                REQUIRE(hull.empty());
            }
        }
    }
}

// -----------------------------------------------------------------------------
// SCENARIO OUTLINE: Построение оболочки для разных конфигураций
// -----------------------------------------------------------------------------
SCENARIO("Jarvis algorithm: building hull for different configurations", "[jarvis][outline]") {
    using TestCase = std::tuple<std::vector<Point>, size_t>;
    
    auto [points, expected_size] = GENERATE(
        TestCase{std::vector<Point>{{0,0}, {100,0}, {100,100}, {0,100}}, 4},
        TestCase{std::vector<Point>{{0,0}, {100,0}, {50,50}, {0,100}}, 3}
    );
    
    JarvisAlgorithm jarvis;
    auto hull = jarvis.findConvexHull(points);
    REQUIRE(hull.size() == expected_size);
}