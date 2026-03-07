#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <chrono>

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
    Point startPoint;
};

namespace jarvis_test {
    extern std::vector<Point> points;
    extern JarvisResult result;
    
    JarvisResult runJarvisAlgorithm(const std::vector<Point>& pts);
    void reset();
}

namespace graham_test {
    extern std::vector<Point> points;
    extern GrahamResult result;
    
    GrahamResult runGrahamAlgorithm(const std::vector<Point>& pts);
    void reset();
}

namespace visualisation_test {
    struct Point2D {
        int x, y;
        bool operator==(const Point2D& other) const;
    };

    struct DisplayState {
        std::vector<Point2D> points;
        std::vector<Point2D> jarvisHull;
        std::vector<Point2D> grahamHull;
        std::string currentFile;
        std::string errorMessage;
        std::string activeAlgorithm;
        bool programRunning;
        bool windowOpen;
        bool axesVisible;
        int pointRadius;
        std::string lastButtonPressed;
    };

    extern DisplayState state;

    void reset();
    std::vector<Point2D> parsePointsFromString(const std::string& str);
    bool isPointInList(const Point2D& p, const std::vector<Point2D>& list);
}

namespace error_handling {
    struct Point2D {
        int x, y;
        bool operator==(const Point2D& other) const;
    };

    struct ErrorState {
        std::vector<Point2D> points;
        std::vector<Point2D> jarvisHull;
        std::string errorMessage;
        bool programRunning;
        bool windowOpen;
        std::string lastButtonPressed;
        std::string currentFile;
    };

    extern ErrorState state;

    void reset();
    bool fileExists(const std::string& filename);
    void createTestFile(const std::string& filename, const std::string& content);
    void deleteTestFile(const std::string& filename);
    bool isPointInList(const Point2D& p, const std::vector<Point2D>& list);
}

namespace performance_testing {
    struct PerformanceState {
        std::vector<Point> points;
        std::vector<Point> grahamHull;
        std::vector<Point> jarvisHull;
        double lastExecutionTimeMs = 0;
        double grahamTimeMs = 0;
        double jarvisTimeMs = 0;
        size_t memoryBefore = 0;
        size_t memoryAfter = 0;
        int expectedHullSize = 0;
        bool profilerActive = false;
        std::chrono::high_resolution_clock::time_point startTime;
    };

    extern PerformanceState state;
}