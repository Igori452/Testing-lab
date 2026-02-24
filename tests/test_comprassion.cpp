#include <gtest/gtest.h>
#include "ConvexHull.h"

// Простой тест для сравнения алгоритмов Джарвиса и Грэхема
TEST(AlgorithmsComparisonTest, JarvisVsGraham)
{
    // Набор точек для тестирования
    std::vector<Point> points = {
        Point(0, 0), Point(100, 0), Point(100, 100), Point(0, 100), Point(50, 50)
    };

    JarvisAlgorithm jarvis;
    GrahamAlgorithm graham;

    auto jarvisHull = jarvis.findConvexHull(points);
    auto grahamHull = graham.findConvexHull(points);

    // Проверка, что оба алгоритма вернули одинаковое количество вершин
    EXPECT_EQ(jarvisHull.size(), grahamHull.size());

    // Проверка, что все вершины из оболочки Джарвиса есть в оболочке Грэхема
    for (const auto& p : jarvisHull) {
        bool found = false;
        for (const auto& q : grahamHull) {
            if (p.x == q.x && p.y == q.y) {
                found = true;
                break;
            }
        }
        EXPECT_TRUE(found) << "Точка (" << p.x << "," << p.y << ") не найдена в оболочке Грэхема";
    }
}