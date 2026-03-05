#include "context.hpp"
#include <algorithm>

// ===== Jarvis контекст =====
namespace jarvis_test {
    std::vector<Point> points;
    JarvisResult result;
    
    JarvisResult runJarvisAlgorithm(const std::vector<Point>& pts) {
        JarvisResult res;
        std::vector<Point> temp = pts;
        JarvisAlgorithm algorithm;
        res.hull = algorithm.findConvexHull(temp);
        res.success = !res.hull.empty();
        res.message = res.success ? "Success" : "Cannot build Convex Hull!";
        return res;
    }
    
    void reset() {
        points.clear();
        result = JarvisResult{};
    }
}

// ===== Graham контекст =====
namespace graham_test {
    std::vector<Point> points;
    GrahamResult result;
    
    GrahamResult runGrahamAlgorithm(const std::vector<Point>& pts) {
        GrahamResult res;
        std::vector<Point> temp = pts;
        GrahamAlgorithm algorithm;
        res.hull = algorithm.findConvexHull(temp);
        res.success = !res.hull.empty();
        res.message = res.success ? "Success" : "Cannot build Convex Hull!";
        
        // Сохраняем стартовую точку (самую нижнюю, левую)
        if (!temp.empty()) {
            res.startPoint = temp[0];
        }
        
        return res;
    }
    
    void reset() {
        points.clear();
        result = GrahamResult{};
    }
}