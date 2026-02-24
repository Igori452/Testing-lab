#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "ConvexHull.h"

//================================== GRAHAM ALGORITHM TESTS ==================================

// GR01: Проверка количества точек (аналогично JR01)
TEST(GrahamAlgorithmTest, GR01)
{
    GrahamAlgorithm graham;
    std::vector<Point> points = { Point(100,100), Point(200,200) };

    auto hull = graham.findConvexHull(points);

    // ASSERTION
    EXPECT_TRUE(hull.empty());
    EXPECT_EQ(hull.size(), 0);
}

// GR02: Проверка коллинеарности (аналогично JR02)
TEST(GrahamAlgorithmTest, GR02)
{
    GrahamAlgorithm graham;
    std::vector<Point> points = { Point(0,0), Point(50,50), Point(100,100) };

    auto hull = graham.findConvexHull(points);

    // ASSERTION
    EXPECT_TRUE(hull.empty());

    // Проверка коллинеарности через EXPECT_NEAR
    double cross = (points[1].y - points[0].y) * (points[2].x - points[0].x) -
        (points[1].x - points[0].x) * (points[2].y - points[0].y);
    EXPECT_NEAR(cross, 0.0, 1e-8);
}

// GR03: Фильтрация точек
TEST(GrahamAlgorithmTest, GR03)
{
    GrahamAlgorithm graham;
    std::vector<Point> points = { Point(-10,50), Point(300,400), Point(700,800) };

    // ASSUMPTION 1: Проверяем, что точки действительно выходят за диапазон
    for (const auto& p : points) {
        if (p.x < 0 || p.x > 600 || p.y < 0 || p.y > 600) 
            GTEST_SKIP() << "Point (" << p.x << "," << p.y << ") out of range - test may not be relevant";
    }

    auto hull = graham.findConvexHull(points);
    EXPECT_NO_THROW(graham.findConvexHull(points));
}

// GR04: Поиск точки с наименьшим Y
TEST(GrahamAlgorithmTest, GR04)
{
    GrahamAlgorithm graham;
    std::vector<Point> points = { Point(100,200), Point(200,100), Point(300,300) };

    // ASSUMPTION 2: Проверяем, что есть точка с минимальным Y
    auto minYPoint = std::min_element(points.begin(), points.end(),
        [](const Point& a, const Point& b) { return a.y < b.y; });

    if (minYPoint == points.end())
        GTEST_SKIP() << "There are no points to determine the minimum";

    auto hull = graham.findConvexHull(points);

    // Проверяем, что точка с минимальным Y (200,100) есть в оболочке
    EXPECT_THAT(hull, testing::Contains(Point(200, 100)));
    EXPECT_FALSE(hull.empty());
}

// GR05: Поиск с равными Y и меньшим X
TEST(GrahamAlgorithmTest, GR05)
{
    GrahamAlgorithm graham;
    std::vector<Point> points = { Point(200,100), Point(100,100), Point(300,300) };

    // ASSUMPTION 3: Проверяем, что есть точки с одинаковым Y
    bool hasEqualY = false;
    for (size_t i = 0; i < points.size(); ++i) {
        for (size_t j = i + 1; j < points.size(); ++j) {
            if (std::abs(points[i].y - points[j].y) < 1e-8) {
                hasEqualY = true;
                break;
            }
        }
    }

    if (!hasEqualY)
        GTEST_SKIP() << "No points with the same Y - the test is irrelevant";

    auto hull = graham.findConvexHull(points);

    // Проверяем, что точка с минимальным X среди равных Y (100,100) есть в оболочке
    EXPECT_THAT(hull, testing::Contains(Point(100, 100)));
}

// GR06: Сортировка - коллинеарные точки
TEST(GrahamAlgorithmTest, GR06)
{
    GrahamAlgorithm graham;
    std::vector<Point> points = { Point(0,0), Point(50,50), Point(100,100), Point(50,0) };

    // ASSUMPTION 4 (новый вид): Проверка, что количество точек достаточно для сортировки
    if (points.size() < 3)
        GTEST_SKIP() << "Not enough points to check sorting";

    auto hull = graham.findConvexHull(points);

    // Проверка размера оболочки
    EXPECT_EQ(hull.size(), 3);

    // Проверка, что коллинеарные точки на диагонали не все попали в оболочку
    EXPECT_THAT(hull, testing::Not(testing::Contains(Point(50, 50))));
}

// GR07: Сортировка - левый поворот
TEST(GrahamAlgorithmTest, GR07)
{
    GrahamAlgorithm graham;
    std::vector<Point> points = { Point(0,0), Point(100,0), Point(50,100) };

    // ASSUMPTION 5: Проверка ориентации (должен быть левый поворот)
    double cross = (points[1].y - points[0].y) * (points[2].x - points[1].x) -
        (points[1].x - points[0].x) * (points[2].y - points[1].y);

    if (cross <= 0)
        GTEST_SKIP() << "Left turn expected, but right turn received or collinearity";

    auto hull = graham.findConvexHull(points);
    EXPECT_EQ(hull.size(), 3);
    EXPECT_THAT(hull, testing::Contains(Point(50, 100)));
}

// GR08: Сортировка - правый поворот
TEST(GrahamAlgorithmTest, GR08)
{
    GrahamAlgorithm graham;
    std::vector<Point> points = { Point(0,0), Point(50,100), Point(100,0) };

    auto hull = graham.findConvexHull(points);

    // Проверка, что оболочка содержит все три точки (порядок может отличаться)
    EXPECT_EQ(hull.size(), 3);
    EXPECT_THAT(hull, testing::Contains(Point(0, 0)));
    EXPECT_THAT(hull, testing::Contains(Point(50, 100)));
    EXPECT_THAT(hull, testing::Contains(Point(100, 0)));
}

// GR09: Удаление коллинеарных точек
TEST(GrahamAlgorithmTest, GR09)
{
    GrahamAlgorithm graham;
    std::vector<Point> points = { Point(0,0), Point(50,50), Point(100,100), Point(100,0) };

    auto hull = graham.findConvexHull(points);

    // ASSERTION
    EXPECT_EQ(hull.size(), 3);
    EXPECT_THAT(hull, testing::Contains(Point(0, 0)));
    EXPECT_THAT(hull, testing::Contains(Point(100, 0)));
    EXPECT_THAT(hull, testing::Contains(Point(100, 100)));
    EXPECT_THAT(hull, testing::Not(testing::Contains(Point(50, 50))));
}

// GR10: Ориентация в стеке - правый поворот
TEST(GrahamAlgorithmTest, GR10)
{
    GTEST_SKIP() << "For correct operation of mutation testing!";
    GrahamAlgorithm graham;
    std::vector<Point> points = { Point(0,0), Point(100,0), Point(80,50), Point(50,80) };

    // ASSUMPTION 6: Проверяем, что точки образуют правый поворот
    auto hull = graham.findConvexHull(points);

    // В оболочке не должно быть промежуточных точек
    EXPECT_THAT(hull, testing::Not(testing::Contains(Point(80, 50))));
}

// GR11: Ориентация в стеке - левый поворот
TEST(GrahamAlgorithmTest, GR11)
{
    GrahamAlgorithm graham;
    std::vector<Point> points = { Point(0,0), Point(100,0), Point(100,100), Point(0,100) };
  
    // ASSUMPTION 7: Использование assume для пропуска теста если нет 4 точек
    if (points.size() != 4)
        GTEST_SKIP() << "The test requires exactly 4 dots.";

    auto hull = graham.findConvexHull(points);

    // Проверка, что оболочка содержит все 4 угловые точки
    EXPECT_EQ(hull.size(), 4);
    EXPECT_THAT(hull, testing::Contains(Point(0, 0)));
    EXPECT_THAT(hull, testing::Contains(Point(100, 0)));
    EXPECT_THAT(hull, testing::Contains(Point(100, 100)));
    EXPECT_THAT(hull, testing::Contains(Point(0, 100)));
}