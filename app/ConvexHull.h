// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#ifndef CONVEX_HULL_H
#define CONVEX_HULL_H

#include <vector>

struct Point {
    int x;
    int y;

    Point() : x(0), y(0) {}
    Point(double x_, double y_) : x(x_), y(y_) {}
    // Оператор сравнения для использования в std::find и EXPECT_EQ
    bool operator==(const Point& other) const;

    // Оператор неравенства (для совместимости с тестами)
    bool operator!=(const Point& other) const;
};

// Интерфейс алгоритма поиска выпуклой оболочки
class ConvexHull {
    public:
        virtual std::vector<Point> findConvexHull(std::vector<Point>& points) = 0;
}; 

// Реализация поиска выпуклой оболочки алгоритмом Джарвиса
class JarvisAlgorithm : public ConvexHull {
    public:
        std::vector<Point> findConvexHull(std::vector<Point>& points) override;
};

// Реализация поиска выпуклой оболочки алгоритмом Грэхэма
class GrahamAlgorithm : public ConvexHull {
public:
    std::vector<Point> findConvexHull(std::vector<Point>& points) override;
};

#endif