#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "ConvexHull.h"

using ::testing::Contains;
using ::testing::Not;
using ::testing::UnorderedElementsAre;

TEST(GrahamStatementTest, TC_GRAHAM_STMT_001_EmptySet)
{
    GrahamAlgorithm graham;
    std::vector<Point> points = {};

    auto hull = graham.findConvexHull(points);

    EXPECT_TRUE(hull.empty());
    EXPECT_EQ(hull.size(), 0);
    EXPECT_NO_THROW(graham.findConvexHull(points));
}

TEST(GrahamStatementTest, TC_GRAHAM_STMT_002_ThreeCollinearPoints)
{
    GrahamAlgorithm graham;
    std::vector<Point> points = { Point(0,0), Point(50,50), Point(100,100) };

    auto hull = graham.findConvexHull(points);

    EXPECT_TRUE(hull.empty());
    EXPECT_EQ(hull.size(), 0);

    double cross = (points[1].y - points[0].y) * (points[2].x - points[0].x) -
        (points[1].x - points[0].x) * (points[2].y - points[0].y);
    EXPECT_NEAR(cross, 0.0, 1e-8);
}

TEST(GrahamStatementTest, TC_GRAHAM_STMT_003_LowestPointSearch)
{
    GrahamAlgorithm graham;
    std::vector<Point> points = {
        Point(300,300),
        Point(100,400),
        Point(500,100),
        Point(200,200),
        Point(150,100)
    };

    auto hull = graham.findConvexHull(points);

    EXPECT_FALSE(hull.empty());
    EXPECT_NO_THROW(graham.findConvexHull(points));

    EXPECT_THAT(hull, Contains(Point(150, 100)));
}

TEST(GrahamStatementTest, TC_GRAHAM_STMT_004_SortingAndStack)
{
    GrahamAlgorithm graham;
    std::vector<Point> points = {
        Point(0,0),
        Point(1,0),
        Point(2,0),
        Point(0,1),
        Point(1,1),
        Point(2,2),
        Point(0,2)
    };

    auto hull = graham.findConvexHull(points);

    EXPECT_FALSE(hull.empty());
    EXPECT_GE(hull.size(), 3);

    EXPECT_THAT(hull, Contains(Point(0, 0)));

    SCOPED_TRACE("Checking sorting and stack construction");
}

TEST(GrahamStatementTest, TC_GRAHAM_STMT_005_Filtering)
{
    GrahamAlgorithm graham;
    std::vector<Point> points = {
        Point(100,100),
        Point(500,100),
        Point(300,400),
        Point(-10,-10),
        Point(700,700)
    };

    auto hull = graham.findConvexHull(points);

    EXPECT_FALSE(hull.empty());

    EXPECT_THAT(hull, Not(Contains(Point(-10, -10))));
    EXPECT_THAT(hull, Not(Contains(Point(700, 700))));
}

TEST(GrahamBranchTest, TC_GRAHAM_BRANCH_001_EmptySet)
{
    GrahamAlgorithm graham;
    std::vector<Point> points = {};

    auto hull = graham.findConvexHull(points);

    EXPECT_TRUE(hull.empty());
    EXPECT_EQ(hull.size(), 0);
}

TEST(GrahamBranchTest, TC_GRAHAM_BRANCH_002_ThreeCollinearPoints)
{
    GrahamAlgorithm graham;
    std::vector<Point> points = { Point(0,0), Point(50,50), Point(100,100) };

    auto hull = graham.findConvexHull(points);

    EXPECT_TRUE(hull.empty());

    double cross = (points[1].y - points[0].y) * (points[2].x - points[0].x) -
        (points[1].x - points[0].x) * (points[2].y - points[0].y);
    EXPECT_NEAR(cross, 0.0, 1e-8);

    SCOPED_TRACE("Checking for branch collinearity");
}

TEST(GrahamBranchTest, TC_GRAHAM_BRANCH_003_ThreeNonCollinearPoints)
{
    GrahamAlgorithm graham;
    std::vector<Point> points = { Point(0,0), Point(100,0), Point(50,100) };

    auto hull = graham.findConvexHull(points);

    EXPECT_FALSE(hull.empty());
    EXPECT_EQ(hull.size(), 3);
    EXPECT_THAT(hull, UnorderedElementsAre(Point(0, 0), Point(100, 0), Point(50, 100)));
}

TEST(GrahamBranchTest, TC_GRAHAM_BRANCH_004_LowestPointSearchWithEqualY)
{
    GrahamAlgorithm graham;
    std::vector<Point> points = {
        Point(200,100),
        Point(100,100),
        Point(300,300)
    };

    auto hull = graham.findConvexHull(points);

    EXPECT_FALSE(hull.empty());
    EXPECT_THAT(hull, Contains(Point(100, 100)));
}

TEST(GrahamBranchTest, TC_GRAHAM_BRANCH_005_CollinearSorting)
{
    GrahamAlgorithm graham;
    std::vector<Point> points = {
        Point(0,0),
        Point(50,50),
        Point(100,100),
        Point(50,0)
    };

    auto hull = graham.findConvexHull(points);

    EXPECT_EQ(hull.size(), 3);
    EXPECT_THAT(hull, Not(Contains(Point(50, 50))));
}

TEST(GrahamBranchTest, TC_GRAHAM_BRANCH_006_NonCollinearSorting)
{
    GrahamAlgorithm graham;
    std::vector<Point> points = {
        Point(0,0),
        Point(100,0),
        Point(50,100)
    };

    auto hull = graham.findConvexHull(points);

    EXPECT_FALSE(hull.empty());
    EXPECT_EQ(hull.size(), 3);
    EXPECT_THAT(hull, Contains(Point(50, 100)));
}

TEST(GrahamBranchTest, TC_GRAHAM_BRANCH_007_RightTurnInStack)
{
    GrahamAlgorithm graham;
    std::vector<Point> points = {
        Point(0,0),
        Point(100,0),
        Point(80,50),
        Point(50,80)
    };

    auto hull = graham.findConvexHull(points);

    EXPECT_NO_THROW(graham.findConvexHull(points));
    EXPECT_FALSE(hull.empty());
}

TEST(GrahamBranchTest, TC_GRAHAM_BRANCH_008_LeftTurnInStack)
{
    GrahamAlgorithm graham;
    std::vector<Point> points = {
        Point(0,0),
        Point(100,0),
        Point(100,100),
        Point(0,100)
    };

    auto hull = graham.findConvexHull(points);

    EXPECT_FALSE(hull.empty());
    EXPECT_EQ(hull.size(), 4);
    EXPECT_THAT(hull, UnorderedElementsAre(Point(0, 0), Point(100, 0), Point(100, 100), Point(0, 100)));
}