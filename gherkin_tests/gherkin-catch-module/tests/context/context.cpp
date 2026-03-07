#include "context.hpp"
#include <algorithm>
#include <fstream>
#include <filesystem>

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

    bool operator<(const Point& a, const Point& b) {
        if (a.x != b.x) return a.x < b.x;
        return a.y < b.y;
    }

    bool operator==(const Point& a, const Point& b) {
        return a.x == b.x && a.y == b.y;
    }
}

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

namespace visualisation_test {
    DisplayState state;

    bool Point2D::operator==(const Point2D& other) const {
        return x == other.x && y == other.y;
    }

    void reset() {
        state.points.clear();
        state.jarvisHull.clear();
        state.grahamHull.clear();
        state.currentFile = "";
        state.errorMessage = "";
        state.activeAlgorithm = "";
        state.programRunning = true;
        state.windowOpen = true;
        state.axesVisible = true;
        state.pointRadius = 3;
        state.lastButtonPressed = "";
    }

    std::vector<Point2D> parsePointsFromString(const std::string& str) {
        std::vector<Point2D> result;
        std::istringstream iss(str);
        int x, y;
        while (iss >> x >> y) {
            result.push_back({ x, y });
        }
        return result;
    }

    bool isPointInList(const Point2D& p, const std::vector<Point2D>& list) {
        return std::find(list.begin(), list.end(), p) != list.end();
    }
}

namespace error_handling {
    ErrorState state;

    bool Point2D::operator==(const Point2D& other) const {
        return x == other.x && y == other.y;
    }

    void reset() {
        state.points.clear();
        state.jarvisHull.clear();
        state.errorMessage = "";
        state.programRunning = true;
        state.windowOpen = true;
        state.lastButtonPressed = "";
    }

    bool fileExists(const std::string& filename) {
        return std::filesystem::exists(filename);
    }

    void createTestFile(const std::string& filename, const std::string& content) {
        std::ofstream file(filename);
        file << content;
        file.close();
    }

    void deleteTestFile(const std::string& filename) {
        std::filesystem::remove(filename);
    }

    bool isPointInList(const Point2D& p, const std::vector<Point2D>& list) {
        return std::find(list.begin(), list.end(), p) != list.end();
    }
}