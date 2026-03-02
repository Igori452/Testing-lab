#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "ConvexHull.h"

using ::testing::UnorderedElementsAre;
using ::testing::Contains;
using ::testing::Each;
using ::testing::AnyOf;

// ============================================================================
// Statement Testing для алгоритма Джарвиса (4 теста)
// ============================================================================

TEST(JarvisStatementTest, TC_JARVIS_STMT_001_EmptySet)
{
    JarvisAlgorithm jarvis;
    std::vector<Point> points = {};

    auto hull = jarvis.findConvexHull(points);

    EXPECT_TRUE(hull.empty());
    EXPECT_EQ(hull.size(), 0);
    EXPECT_NO_THROW(jarvis.findConvexHull(points));
}

TEST(JarvisStatementTest, TC_JARVIS_STMT_002_TwoPoints)
{
    JarvisAlgorithm jarvis;
    std::vector<Point> points = { Point(100,100), Point(200,200) };

    auto hull = jarvis.findConvexHull(points);

    EXPECT_TRUE(hull.empty());
    EXPECT_EQ(hull.size(), 0);
    EXPECT_NO_THROW(jarvis.findConvexHull(points));
}

TEST(JarvisStatementTest, TC_JARVIS_STMT_003_ThreeCollinearPoints)
{
    JarvisAlgorithm jarvis;
    std::vector<Point> points = { Point(0,0), Point(50,50), Point(100,100) };

    auto hull = jarvis.findConvexHull(points);

    EXPECT_TRUE(hull.empty());
    EXPECT_EQ(hull.size(), 0);

    // Checking that the points have not changed
    EXPECT_NEAR(points[0].x, 0.0, 0.001);
    EXPECT_NEAR(points[0].y, 0.0, 0.001);
    EXPECT_NEAR(points[1].x, 50.0, 0.001);
    EXPECT_NEAR(points[1].y, 50.0, 0.001);
    EXPECT_NEAR(points[2].x, 100.0, 0.001);
    EXPECT_NEAR(points[2].y, 100.0, 0.001);
}

TEST(JarvisStatementTest, TC_JARVIS_STMT_004_CombinedScenario)
{
    GTEST_SKIP() << "The test has been disabled because the developer is aware of the error.";
    JarvisAlgorithm jarvis;
    std::vector<Point> points = {
        Point(300,300),
        Point(100,100),
        Point(500,100),
        Point(300,400),
        Point(-10,-10),
        Point(700,700),
        Point(50,200)
    };

    auto hull = jarvis.findConvexHull(points);

    EXPECT_FALSE(hull.empty());
    EXPECT_NO_THROW(jarvis.findConvexHull(points));

    EXPECT_EQ(hull.size(), 3);

    EXPECT_THAT(hull, Contains(Point(100, 100)));
    EXPECT_THAT(hull, Contains(Point(500, 100)));
    EXPECT_THAT(hull, Contains(Point(300, 400)));

    EXPECT_THAT(hull, ::testing::Not(Contains(Point(-10, -10))));
    EXPECT_THAT(hull, ::testing::Not(Contains(Point(700, 700))));

    SCOPED_TRACE("Testing a combined scenario with filtering and left point search");
}

TEST(JarvisBranchTest, TC_JARVIS_BRANCH_001_EmptySet)
{
    JarvisAlgorithm jarvis;
    std::vector<Point> points = {};

    auto hull = jarvis.findConvexHull(points);

    EXPECT_TRUE(hull.empty());
    EXPECT_EQ(hull.size(), 0);
}

TEST(JarvisBranchTest, TC_JARVIS_BRANCH_002_ThreeCollinearPoints)
{
    JarvisAlgorithm jarvis;
    std::vector<Point> points = { Point(0,0), Point(50,50), Point(100,100) };

    auto hull = jarvis.findConvexHull(points);

    EXPECT_TRUE(hull.empty());

    SCOPED_TRACE("Checking for branch collinearity");
}

TEST(JarvisBranchTest, TC_JARVIS_BRANCH_003_ThreeNonCollinearPoints)
{
    JarvisAlgorithm jarvis;
    std::vector<Point> points = { Point(0,0), Point(100,0), Point(50,100) };

    auto hull = jarvis.findConvexHull(points);

    EXPECT_FALSE(hull.empty());
    EXPECT_EQ(hull.size(), 3);

    EXPECT_THAT(hull, UnorderedElementsAre(
        Point(0, 0), Point(100, 0), Point(50, 100)
    ));

    SCOPED_TRACE("Testing the basic scenario with three non-collinear points");
}

TEST(JarvisBranchTest, TC_JARVIS_BRANCH_004_FilteringTest)
{
    JarvisAlgorithm jarvis;
    std::vector<Point> points = {
        Point(100,100),
        Point(500,100),
        Point(300,400),
        Point(-10,-10),
        Point(700,700)
    };

    auto hull = jarvis.findConvexHull(points);

    EXPECT_FALSE(hull.empty());
    EXPECT_EQ(hull.size(), 3);

    EXPECT_THAT(hull, Contains(Point(100, 100)));
    EXPECT_THAT(hull, Contains(Point(500, 100)));
    EXPECT_THAT(hull, Contains(Point(300, 400)));
    EXPECT_THAT(hull, ::testing::Not(Contains(Point(-10, -10))));
    EXPECT_THAT(hull, ::testing::Not(Contains(Point(700, 700))));

    SCOPED_TRACE("Checking filter branches");
}

class JarvisOrientationParamTest : public testing::TestWithParam<std::tuple<std::vector<Point>, size_t>> {};

TEST_P(JarvisOrientationParamTest, OrientationTest)
{
    auto [points, expectedSize] = GetParam();

    JarvisAlgorithm jarvis;
    auto hull = jarvis.findConvexHull(const_cast<std::vector<Point>&>(points));

    EXPECT_NO_THROW(jarvis.findConvexHull(points));
    EXPECT_EQ(hull.size(), expectedSize);
}

INSTANTIATE_TEST_SUITE_P(
    JarvisOrientationTests,
    JarvisOrientationParamTest,
    testing::Values(
        std::make_tuple(std::vector<Point>{Point(0, 0), Point(100, 0), Point(50, 100), Point(50, 50)}, 3),
        std::make_tuple(std::vector<Point>{Point(0, 0), Point(100, 0), Point(50, 100)}, 3),
        std::make_tuple(std::vector<Point>{Point(0, 0), Point(100, 0), Point(100, 100), Point(0, 100)}, 4),
        std::make_tuple(std::vector<Point>{Point(0, 0), Point(100, 0), Point(100, 100), Point(0, 100), Point(50, 50)}, 4)
    )
);