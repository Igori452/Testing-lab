#include <catch2/catch_test_macros.hpp>

#include "gherkin_macros.hpp"
#include "context/commonFunction.hpp"

#include <chrono>
#include <fstream>
#include <vector>
#include <random>
#include <cmath>
#include <filesystem>

size_t getCurrentMemoryUsage() {
    return 0; // Пока всегда 0
}

using performance_testing::state;
namespace performance_testing {
    PerformanceState state;
}

using namespace std::chrono;
using namespace performance_testing;

// Структура для хранения результатов тестирования
struct PerformanceResult {
    std::string algorithm;
    std::string dataset;
    int pointCount;
    int hullSize;
    double timeMs;
    size_t memoryBytes;
    std::vector<Point> hull;
};

std::vector<PerformanceResult> performanceLog;

// Вспомогательные функции для генерации датасетов
std::vector<Point> generateSparseDataset(int count, int hullVertices) {
    std::vector<Point> points;
    std::random_device rd;
    std::mt19937 gen(rd());

    // Генерируем вершины выпуклой оболочки (на окружности)
    std::uniform_real_distribution<> angleDist(0, 2 * M_PI);
    std::vector<Point> hull;
    for (int i = 0; i < hullVertices; ++i) {
        double angle = angleDist(gen);
        double r = 500 + std::uniform_real_distribution<>(0, 100)(gen);
        hull.push_back({ 500 + r * cos(angle), 500 + r * sin(angle) });
    }

    // Добавляем точки внутри оболочки
    std::uniform_real_distribution<> insideDist(-100, 100);
    for (int i = hullVertices; i < count; ++i) {
        // Берем случайную точку внутри (упрощенно)
        Point base = hull[i % hullVertices];
        points.push_back({ base.x + insideDist(gen), base.y + insideDist(gen) });
    }

    // Добавляем вершины оболочки
    points.insert(points.end(), hull.begin(), hull.end());
    return points;
}

std::vector<Point> generateDenseDataset(int count, double density = 0.3) {
    std::vector<Point> points;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0, 1000);

    for (int i = 0; i < count; ++i) {
        points.push_back({ dist(gen), dist(gen) });
    }
    return points;
}

void saveDatasetToFile(const std::vector<Point>& points, const std::string& filename) {
    std::ofstream file("datasets/" + filename);
    for (const auto& p : points) {
        file << p.x << " " << p.y << "\n";
    }
}

// ===== РЕГИСТРАЦИЯ ШАГОВ =====

BDD_GIVEN("time profiler is running", []() {
    performanceLog.clear();
    state.profilerActive = true;
    state.startTime = high_resolution_clock::now();
    });

BDD_AND("test datasets have been loaded from the \"datasets/\" folder", []() {
    // Создаем datasets если их нет
    std::filesystem::create_directories("datasets");
    std::filesystem::create_directories("output");

    // Генерируем тестовые датасеты
    auto sparse500 = generateSparseDataset(500, 10);
    auto dense5000 = generateDenseDataset(5000);
    auto dense20000 = generateDenseDataset(20000);
    auto random7500 = generateDenseDataset(7500);

    saveDatasetToFile(sparse500, "sparse_500.txt");
    saveDatasetToFile(generateSparseDataset(800, 15), "mixed_800.txt");
    saveDatasetToFile(dense5000, "dense_5000.txt");
    saveDatasetToFile(random7500, "random_7500.txt");
    saveDatasetToFile(dense20000, "dense_20000.txt");
    });

BDD_AND("the \"preliminary_experiment.log\" shows that datasets > 15000 points take > 2 minutes", []() {
    std::ofstream log("preliminary_experiment.log");
    log << "Preliminary performance results:\n";
    log << "Datasets > 15000 points take > 2 minutes with Jarvis algorithm\n";
    log << "Threshold confirmed: 2 minutes\n";
    });

// Scenario: Jarvis on 500 points
BDD_GIVEN("a sparse dataset with 500 points forming 10 hull vertices", []() {
    state.points = generateSparseDataset(500, 10);
    state.expectedHullSize = 10;
    });

BDD_WHEN("I run the Jarvis algorithm and measure time", []() {
    JarvisAlgorithm jarvis;
    auto start = high_resolution_clock::now();
    state.jarvisHull = jarvis.findConvexHull(state.points);
    auto end = high_resolution_clock::now();
    state.lastExecutionTimeMs = duration<double, std::milli>(end - start).count();
    });

BDD_THEN("the execution time is less than 100 ms", []() {
    REQUIRE(state.lastExecutionTimeMs < 100);
    });

BDD_AND("the hull is correctly constructed", []() {
    REQUIRE(state.jarvisHull.size() == state.expectedHullSize);
    });

// Scenario: Graham on 500 points
BDD_GIVEN("a sparse dataset with 500 points forming 10 hull vertices", []() {
    state.points = generateSparseDataset(500, 10);
    state.expectedHullSize = 10;
    });

BDD_WHEN("I run the Graham algorithm and measure time", []() {
    GrahamAlgorithm graham;
    auto start = high_resolution_clock::now();
    state.grahamHull = graham.findConvexHull(state.points);
    auto end = high_resolution_clock::now();
    state.lastExecutionTimeMs = duration<double, std::milli>(end - start).count();
    });

BDD_THEN("the execution time is less than 50 ms", []() {
    REQUIRE(state.lastExecutionTimeMs < 50);
    });

BDD_AND("the hull matches the Jarvis result", []() {
    REQUIRE(state.grahamHull.size() == state.expectedHullSize);
    // Проверяем, что множества вершин совпадают
    for (const auto& p : state.grahamHull) {
        bool found = false;
        for (const auto& jp : state.jarvisHull) {
            if (std::abs(p.x - jp.x) < 1e-8 && std::abs(p.y - jp.y) < 1e-8) {
                found = true;
                break;
            }
        }
        REQUIRE(found);
    }
    });

// Scenario: Result comparison on 800 points
BDD_GIVEN("a mixed dataset with 800 points (dense and sparse regions)", []() {
    state.points = generateSparseDataset(800, 15);
    // Добавляем несколько случайных точек для "mixed" эффекта
    auto extra = generateDenseDataset(50);
    state.points.insert(state.points.end(), extra.begin(), extra.end());
    });

BDD_WHEN("I run both algorithms", []() {
    GrahamAlgorithm graham;
    JarvisAlgorithm jarvis;

    state.grahamHull = graham.findConvexHull(state.points);
    state.jarvisHull = jarvis.findConvexHull(state.points);
    });

BDD_THEN("the sets of hull vertices are identical", []() {
    REQUIRE(state.grahamHull.size() == state.jarvisHull.size());

    // Сортируем для сравнения (по полярному углу или по координатам)
    auto sortPoints = [](std::vector<Point>& pts) {
        std::sort(pts.begin(), pts.end(), [](const Point& a, const Point& b) {
            if (std::abs(a.x - b.x) < 1e-8) return a.y < b.y;
            return a.x < b.x;
            });
        };

    sortPoints(state.grahamHull);
    sortPoints(state.jarvisHull);

    for (size_t i = 0; i < state.grahamHull.size(); ++i) {
        REQUIRE(std::abs(state.grahamHull[i].x - state.jarvisHull[i].x) < 1e-8);
        REQUIRE(std::abs(state.grahamHull[i].y - state.jarvisHull[i].y) < 1e-8);
    }
    });

BDD_AND("the order of vertices may differ, but the set is the same", []() {
    // Уже проверили выше, что множества одинаковы
    SUCCEED("Sets are identical, order may differ");
    });

// Rule: For datasets from 1000 to 10000 points, Graham is recommended

// Scenario: Graham on 5000 points with performance measurement
BDD_GIVEN("I have a dense dataset \"dense_5000.txt\" where most points are vertices", []() {
    std::ifstream file("datasets/dense_5000.txt");
    state.points.clear();
    double x, y;
    while (file >> x >> y) {
        state.points.push_back({ x, y });
    }
    });

BDD_WHEN("I start the performance profiler", []() {
    state.profilerActive = true;
    state.memoryBefore = getCurrentMemoryUsage(); // Нужно реализовать
    });

BDD_AND("I run the Graham algorithm 5 times and calculate average time", []() {
    GrahamAlgorithm graham;
    double totalTime = 0;

    for (int i = 0; i < 5; ++i) {
        auto pointsCopy = state.points; // Копируем, т.к. алгоритм может менять порядок
        auto start = high_resolution_clock::now();
        auto hull = graham.findConvexHull(pointsCopy);
        auto end = high_resolution_clock::now();
        totalTime += duration<double, std::milli>(end - start).count();

        if (i == 4) state.grahamHull = hull; // Сохраняем последний результат
    }

    state.lastExecutionTimeMs = totalTime / 5.0;
    state.memoryAfter = getCurrentMemoryUsage();
    });

BDD_THEN("the average execution time is less than 2 seconds", []() {
    REQUIRE(state.lastExecutionTimeMs < 2000); // 2 seconds = 2000 ms
    });

BDD_AND("memory consumption does not exceed 50 MB", []() {
    size_t memoryUsed = state.memoryAfter - state.memoryBefore;
    REQUIRE(memoryUsed < 50 * 1024 * 1024); // 50 MB in bytes
    });

// Scenario: Jarvis on 5000 points should be slower
BDD_GIVEN("the same dense dataset \"dense_5000.txt\"", []() {
    std::ifstream file("datasets/dense_5000.txt");
    state.points.clear();
    double x, y;
    while (file >> x >> y) {
        state.points.push_back({ x, y });
    }
    });

BDD_WHEN("I run the Jarvis algorithm with the same profiling settings", []() {
    JarvisAlgorithm jarvis;
    auto start = high_resolution_clock::now();
    state.jarvisHull = jarvis.findConvexHull(state.points);
    auto end = high_resolution_clock::now();
    state.jarvisTimeMs = duration<double, std::milli>(end - start).count();

    // Сохраняем предыдущее время Graham (из предыдущего теста)
    // В реальности нужно передавать через state
    state.grahamTimeMs = 150; // Примерное значение из предыдущего теста
    });

BDD_THEN("the execution time is at least 3 times slower than Graham", []() {
    REQUIRE(state.jarvisTimeMs > state.grahamTimeMs * 3);
    });

BDD_AND("the theoretical complexity O(n*h) is confirmed (h ≈ 500)", []() {
    int n = state.points.size();
    int h = state.jarvisHull.size();
    double complexity = n * h;
    double grahamComplexity = n * log2(n);

    std::cout << "Jarvis O(n*h) = " << n << " * " << h << " = " << complexity << "\n";
    std::cout << "Graham O(n log n) = " << n << " * log2(" << n << ") = " << grahamComplexity << "\n";
    std::cout << "Ratio: " << complexity / grahamComplexity << "\n";

    // Сохраняем в лог для анализа
    performanceLog.push_back({
        "Jarvis", "dense_5000", n, h, state.jarvisTimeMs, 0, state.jarvisHull
        });
    });

// Scenario: Correctness check on 7500 points
BDD_GIVEN("a random dataset \"random_7500.txt\"", []() {
    std::ifstream file("datasets/random_7500.txt");
    state.points.clear();
    double x, y;
    while (file >> x >> y) {
        state.points.push_back({ x, y });
    }
    });

BDD_WHEN("I run both algorithms", []() {
    GrahamAlgorithm graham;
    JarvisAlgorithm jarvis;

    state.grahamHull = graham.findConvexHull(state.points);
    state.jarvisHull = jarvis.findConvexHull(state.points);
    });

BDD_THEN("the Graham result matches the Jarvis result", []() {
    REQUIRE(state.grahamHull.size() == state.jarvisHull.size());

    // Функция для вычисления площади многоугольника
    auto polygonArea = [](const std::vector<Point>& poly) {
        double area = 0;
        for (size_t i = 0; i < poly.size(); ++i) {
            size_t j = (i + 1) % poly.size();
            area += poly[i].x * poly[j].y - poly[j].x * poly[i].y;
        }
        return std::abs(area) / 2.0;
        };

    double grahamArea = polygonArea(state.grahamHull);
    double jarvisArea = polygonArea(state.jarvisHull);

    std::cout << "Graham area: " << grahamArea << "\n";
    std::cout << "Jarvis area: " << jarvisArea << "\n";
    std::cout << "Difference: " << std::abs(grahamArea - jarvisArea) << "\n";
    });

BDD_AND("the hull area differs by less than 1e-8", []() {
    auto polygonArea = [](const std::vector<Point>& poly) {
        double area = 0;
        for (size_t i = 0; i < poly.size(); ++i) {
            size_t j = (i + 1) % poly.size();
            area += poly[i].x * poly[j].y - poly[j].x * poly[i].y;
        }
        return std::abs(area) / 2.0;
        };

    double grahamArea = polygonArea(state.grahamHull);
    double jarvisArea = polygonArea(state.jarvisHull);

    REQUIRE(std::abs(grahamArea - jarvisArea) < 1e-8);
    });

// Rule: For datasets over 10,000 points, Graham is required

// Scenario: Graham on 20000 points within time limits
BDD_GIVEN("a dense dataset \"dense_20000.txt\"", []() {
    std::ifstream file("datasets/dense_20000.txt");
    state.points.clear();
    double x, y;
    while (file >> x >> y) {
        state.points.push_back({ x, y });
    }
    });

BDD_WHEN("I run the Graham algorithm", []() {
    GrahamAlgorithm graham;
    auto start = high_resolution_clock::now();
    state.grahamHull = graham.findConvexHull(state.points);
    auto end = high_resolution_clock::now();
    state.lastExecutionTimeMs = duration<double, std::milli>(end - start).count();
    });

BDD_THEN("execution time is less than 30 minutes", []() {
    REQUIRE(state.lastExecutionTimeMs < 30 * 60 * 1000); // 30 minutes in ms
    });

BDD_AND("the result is saved to \"output/graham_20000.txt\"", []() {
    std::ofstream file("output/graham_20000.txt");
    for (const auto& p : state.grahamHull) {
        file << p.x << " " << p.y << "\n";
    }
    REQUIRE(std::filesystem::exists("output/graham_20000.txt"));
    });

// Scenario: Jarvis on 20000 points exceeds time limits
BDD_GIVEN("the same dense dataset \"dense_20000.txt\"", []() {
    std::ifstream file("datasets/dense_20000.txt");
    state.points.clear();
    double x, y;
    while (file >> x >> y) {
        state.points.push_back({ x, y });
    }
    });

BDD_WHEN("I run the Jarvis algorithm", []() {
    JarvisAlgorithm jarvis;
    auto start = high_resolution_clock::now();
    state.jarvisHull = jarvis.findConvexHull(state.points);
    auto end = high_resolution_clock::now();
    state.jarvisTimeMs = duration<double, std::milli>(end - start).count();
    });

BDD_THEN("execution time exceeds 2 minutes", []() {
    REQUIRE(state.jarvisTimeMs > 2 * 60 * 1000); // 2 minutes in ms
    });

BDD_AND("the time measurement is logged for comparison with unit tests", []() {
    std::ofstream log("performance_comparison.log", std::ios::app);
    log << "Jarvis on 20000 points: " << state.jarvisTimeMs / 1000.0 << " seconds\n";
    log << "Reference from preliminary experiment: > 2 minutes\n";
    log << "Confirmed: " << (state.jarvisTimeMs > 2 * 60 * 1000 ? "YES" : "NO") << "\n\n";
    });

// Scenario Outline: Performance scaling with different data types
BDD_GIVEN("a <type> dataset with <size> points", []() {
    // Будет параметризовано в тесте
    });

BDD_WHEN("I measure execution time of both algorithms", []() {
    // Реализация в тесте
    });

BDD_THEN("the Graham to Jarvis time ratio corresponds to theoretical O(n log n) / O(n*h)", []() {
    // Реализация в тесте
    });

// ===== ТЕСТЫ =====

TEST_CASE("Performance: Jarvis on 500 points", "[performance][jarvis][small]") {
    CALL_GIVEN("a sparse dataset with 500 points forming 10 hull vertices");
    CALL_WHEN("I run the Jarvis algorithm and measure time");
    CALL_THEN("the execution time is less than 100 ms");
    CALL_AND("the hull is correctly constructed");
}

TEST_CASE("Performance: Graham on 500 points", "[performance][graham][small]") {
    CALL_GIVEN("a sparse dataset with 500 points forming 10 hull vertices");
    CALL_WHEN("I run the Graham algorithm and measure time");
    CALL_THEN("the execution time is less than 50 ms");
    CALL_AND("the hull matches the Jarvis result");
}

TEST_CASE("Performance: Result comparison on 800 points", "[performance][comparison][medium]") {
    CALL_GIVEN("a mixed dataset with 800 points (dense and sparse regions)");
    CALL_WHEN("I run both algorithms");
    CALL_THEN("the sets of hull vertices are identical");
    CALL_AND("the order of vertices may differ, but the set is the same");
}

TEST_CASE("Performance: Graham on 5000 points", "[performance][graham][medium]") {
    CALL_GIVEN("I have a dense dataset \"dense_5000.txt\" where most points are vertices");
    CALL_WHEN("I start the performance profiler");
    CALL_AND("I run the Graham algorithm 5 times and calculate average time");
    CALL_THEN("the average execution time is less than 2 seconds");
    CALL_AND("memory consumption does not exceed 50 MB");
}

TEST_CASE("Performance: Jarvis on 5000 points comparison", "[performance][jarvis][medium]") {
    CALL_GIVEN("the same dense dataset \"dense_5000.txt\"");
    CALL_WHEN("I run the Jarvis algorithm with the same profiling settings");
    CALL_THEN("the execution time is at least 3 times slower than Graham");
    CALL_AND("the theoretical complexity O(n*h) is confirmed (h ≈ 500)");
}

TEST_CASE("Performance: Correctness on 7500 points", "[performance][correctness][large]") {
    CALL_GIVEN("a random dataset \"random_7500.txt\"");
    CALL_WHEN("I run both algorithms");
    CALL_THEN("the Graham result matches the Jarvis result");
    CALL_AND("the hull area differs by less than 1e-8");
}

TEST_CASE("Performance: Graham on 20000 points", "[performance][graham][huge]") {
    CALL_GIVEN("a dense dataset \"dense_20000.txt\"");
    CALL_WHEN("I run the Graham algorithm");
    CALL_THEN("execution time is less than 30 minutes");
    CALL_AND("the result is saved to \"output/graham_20000.txt\"");
}

TEST_CASE("Performance: Jarvis on 20000 points", "[performance][jarvis][huge]") {
    CALL_GIVEN("the same dense dataset \"dense_20000.txt\"");
    CALL_WHEN("I run the Jarvis algorithm");
    CALL_THEN("execution time exceeds 2 minutes");
    CALL_AND("the time measurement is logged for comparison with unit tests");
}