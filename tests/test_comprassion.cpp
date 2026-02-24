#include <gtest/gtest.h>
#include "ConvexHull.h"

TEST(AlgorithmsComparisonTest, JarvisVsGraham)
{
    std::vector<Point> points = {
        Point(0, 0), Point(100, 0), Point(100, 100), Point(0, 100), Point(50, 50)
    };

    JarvisAlgorithm jarvis;
    GrahamAlgorithm graham;

    auto jarvisHull = jarvis.findConvexHull(points);
    auto grahamHull = graham.findConvexHull(points);

    EXPECT_EQ(jarvisHull.size(), grahamHull.size());

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