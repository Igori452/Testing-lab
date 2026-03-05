#pragma once
#include "context.hpp"
#include <algorithm>
#include <sstream>

inline bool isPointInVector(const Point& p, const std::vector<Point>& vec) {
    return std::find(vec.begin(), vec.end(), p) != vec.end();
}

inline bool arePointsEqual(const Point& a, const Point& b) {
    const double eps = 1e-8;
    return std::abs(a.x - b.x) < eps && std::abs(a.y - b.y) < eps;
}

inline std::vector<Point> parsePointsFromString(const std::string& str) {
    std::vector<Point> result;
    std::istringstream iss(str);
    double x, y;
    while (iss >> x >> y) {
        result.push_back({ x, y });
    }
    return result;
}