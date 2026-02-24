#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "ConvexHull.h"

TEST(GrahamAlgorithmTest, GR01)
{
    GrahamAlgorithm graham;
    std::vector<Point> points = { Point(100,100), Point(200,200) };

    auto hull = graham.findConvexHull(points);

    // ASSERTION
    EXPECT_TRUE(hull.empty());
    EXPECT_EQ(hull.size(), 0);
}

TEST(GrahamAlgorithmTest, GR02)
{
    GrahamAlgorithm graham;
    std::vector<Point> points = { Point(0,0), Point(50,50), Point(100,100) };

    auto hull = graham.findConvexHull(points);

    // ASSERTION
    EXPECT_TRUE(hull.empty());

    double cross = (points[1].y - points[0].y) * (points[2].x - points[0].x) -
        (points[1].x - points[0].x) * (points[2].y - points[0].y);
    EXPECT_NEAR(cross, 0.0, 1e-8);
}

TEST(GrahamAlgorithmTest, GR03)
{
    GrahamAlgorithm graham;
    std::vector<Point> points = { Point(-10,50), Point(300,400), Point(700,800) };

    // ASSUMPTION
    for (const auto& p : points) {
        if (p.x < 0 || p.x > 600 || p.y < 0 || p.y > 600) 
            GTEST_SKIP() << "Point (" << p.x << "," << p.y << ") out of range - test may not be relevant";
    }

    auto hull = graham.findConvexHull(points);
    EXPECT_NO_THROW(graham.findConvexHull(points));
}

TEST(GrahamAlgorithmTest, GR04)
{
    GrahamAlgorithm graham;
    std::vector<Point> points = { Point(100,200), Point(200,100), Point(300,300) };

    // ASSUMPTION
    auto minYPoint = std::min_element(points.begin(), points.end(),
        [](const Point& a, const Point& b) { return a.y < b.y; });

    if (minYPoint == points.end())
        GTEST_SKIP() << "There are no points to determine the minimum";

    auto hull = graham.findConvexHull(points);

    EXPECT_THAT(hull, testing::Contains(Point(200, 100)));
    EXPECT_FALSE(hull.empty());
}

TEST(GrahamAlgorithmTest, GR05)
{
    GrahamAlgorithm graham;
    std::vector<Point> points = { Point(200,100), Point(100,100), Point(300,300) };

    // ASSUMPTION 3
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

    EXPECT_THAT(hull, testing::Contains(Point(100, 100)));
}

TEST(GrahamAlgorithmTest, GR06)
{
    GrahamAlgorithm graham;
    std::vector<Point> points = { Point(0,0), Point(50,50), Point(100,100), Point(50,0) };

    // ASSUMPTION 
    if (points.size() < 3)
        GTEST_SKIP() << "Not enough points to check sorting";

    auto hull = graham.findConvexHull(points);

    EXPECT_EQ(hull.size(), 3);

    EXPECT_THAT(hull, testing::Not(testing::Contains(Point(50, 50))));
}

TEST(GrahamAlgorithmTest, GR07)
{
    GrahamAlgorithm graham;
    std::vector<Point> points = { Point(0,0), Point(100,0), Point(50,100) };

    // ASSUMPTION
    double cross = (points[1].y - points[0].y) * (points[2].x - points[1].x) -
        (points[1].x - points[0].x) * (points[2].y - points[1].y);

    if (cross <= 0)
        GTEST_SKIP() << "Left turn expected, but right turn received or collinearity";

    auto hull = graham.findConvexHull(points);
    EXPECT_EQ(hull.size(), 3);
    EXPECT_THAT(hull, testing::Contains(Point(50, 100)));
}

TEST(GrahamAlgorithmTest, GR08)
{
    GrahamAlgorithm graham;
    std::vector<Point> points = { Point(0,0), Point(50,100), Point(100,0) };

    auto hull = graham.findConvexHull(points);

    EXPECT_EQ(hull.size(), 3);
    EXPECT_THAT(hull, testing::Contains(Point(0, 0)));
    EXPECT_THAT(hull, testing::Contains(Point(50, 100)));
    EXPECT_THAT(hull, testing::Contains(Point(100, 0)));
}

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

TEST(GrahamAlgorithmTest, GR10)
{
    //GTEST_SKIP() << "For correct operation of mutation testing!";
    GrahamAlgorithm graham;
    std::vector<Point> points = { Point(0,0), Point(100,0), Point(80,50), Point(50,80) };

    // ASSUMPTION
    auto hull = graham.findConvexHull(points);

    EXPECT_THAT(hull, testing::Not(testing::Contains(Point(80, 50))));
}

TEST(GrahamAlgorithmTest, GR11)
{
    GrahamAlgorithm graham;
    std::vector<Point> points = { Point(0,0), Point(100,0), Point(100,100), Point(0,100) };
  
    // ASSUMPTION
    if (points.size() != 4)
        GTEST_SKIP() << "The test requires exactly 4 dots.";

    auto hull = graham.findConvexHull(points);

    EXPECT_EQ(hull.size(), 4);
    EXPECT_THAT(hull, testing::Contains(Point(0, 0)));
    EXPECT_THAT(hull, testing::Contains(Point(100, 0)));
    EXPECT_THAT(hull, testing::Contains(Point(100, 100)));
    EXPECT_THAT(hull, testing::Contains(Point(0, 100)));
}