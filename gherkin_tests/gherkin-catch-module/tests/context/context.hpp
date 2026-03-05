#pragma once
#include <string>
#include <vector>
#include "ConvexHull.h"

struct JarvisResult {
    std::vector<Point> hull;
    std::string message;
    bool success;
};

struct GrahamResult {
    std::vector<Point> hull;
    std::string message;
    bool success;
    Point startPoint;  // Для проверки стартовой точки
};

// Глобальный контекст для тестов Jarvis
namespace jarvis_test {
    extern std::vector<Point> points;
    extern JarvisResult result;
    
    JarvisResult runJarvisAlgorithm(const std::vector<Point>& pts);
    void reset();
}

// Глобальный контекст для тестов Graham
namespace graham_test {
    extern std::vector<Point> points;
    extern GrahamResult result;
    
    GrahamResult runGrahamAlgorithm(const std::vector<Point>& pts);
    void reset();
}