#include <cstdint>
#include <cstddef>
#include <cstring>
#include <vector>
#include <cmath>
#include "ConvexHull.h"

// Минимальная функция для фаззинга
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    // Нужно минимум 16 байт для одной точки (2 double)
    if (size < 16) return 0;
    
    // Создаем точки из входных данных
    std::vector<Point> points;
    size_t numPoints = size / 16;
    
    for (size_t i = 0; i < numPoints; ++i) {
        double x, y;
        memcpy(&x, data + i * 16, 8);
        memcpy(&y, data + i * 16 + 8, 8);
        
        // Убираем NaN и Inf, чтобы они не мешали
        if (std::isnan(x) || std::isinf(x)) x = 0;
        if (std::isnan(y) || std::isinf(y)) y = 0;
        
        points.push_back({x, y});
    }
    
    // Тестируем Jarvis
    {
        std::vector<Point> p = points;
        JarvisAlgorithm jarvis;
        jarvis.findConvexHull(p);
    }
    
    // Тестируем Graham
    {
        std::vector<Point> p = points;
        GrahamAlgorithm graham;
        graham.findConvexHull(p);
    }
    
    return 0;
}