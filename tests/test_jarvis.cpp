#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "ConvexHull.h"

TEST(JarvisAlgorithmTest, JR01)
{
    JarvisAlgorithm jarvis;
    std::vector<Point> points = { Point(100,100), Point(200,200) };

    auto hull = jarvis.findConvexHull(points);

    // ASSERTION
    EXPECT_EQ(hull.size(), 0);
}

TEST(JarvisAlgorithmTest, JR02)
{
    JarvisAlgorithm jarvis;
    std::vector<Point> points = { Point(0,0), Point(50,50), Point(100,100) };

    auto hull = jarvis.findConvexHull(points);

    // ASSERTION
    EXPECT_TRUE(hull.empty());

    for (const auto& p : points) {
        EXPECT_NEAR(p.x, p.x, 0.001);
        EXPECT_NEAR(p.y, p.y, 0.001);
    }
}

TEST(JarvisAlgorithmTest, JR03)
{
    JarvisAlgorithm jarvis;
    std::vector<Point> points = { Point(-10,50), Point(300,400), Point(700,800) };

    auto hull = jarvis.findConvexHull(points);

    // ASSERTION
    EXPECT_NO_THROW(jarvis.findConvexHull(points));
    EXPECT_TRUE(jarvis.findConvexHull(points).empty());
}

TEST(JarvisAlgorithmTest, JR04)
{
    //GTEST_SKIP() << "For correct operation of mutation testing!";
    JarvisAlgorithm jarvis;
    std::vector<Point> points = { Point(200,200), Point(100,100), Point(300,300) };

    auto hull = jarvis.findConvexHull(points);

    EXPECT_FALSE(hull.empty());

    EXPECT_THAT(hull, testing::Contains(Point(100, 100)));
}

class OrientationParamTest : public testing::TestWithParam<std::tuple<
    std::vector<Point>,
    bool,
    std::string
    >> {};

TEST_P(OrientationParamTest, JR05andJR06)
{
    auto [points, shouldUpdate, description] = GetParam();

    JarvisAlgorithm jarvis;
    auto hull = jarvis.findConvexHull(const_cast<std::vector<Point>&>(points));

    SCOPED_TRACE(description);

    EXPECT_NO_THROW(jarvis.findConvexHull(points));

    if (shouldUpdate) {
        EXPECT_FALSE(hull.empty());
    }
}

INSTANTIATE_TEST_SUITE_P(
    JarvisOrientationTests,
    OrientationParamTest,
    testing::Combine(
        testing::Values(
            std::vector<Point>{Point(0, 0), Point(100, 0), Point(50, 100), Point(50, 50)},
            std::vector<Point>{Point(0, 0), Point(100, 0), Point(50, 100)}
        ),
        testing::Values(true, false),
        testing::Values("JR05: should update index", "JR06: should not update index")
    )
);

TEST(JarvisAlgorithmTest, JR07)
{
    JarvisAlgorithm jarvis;
    std::vector<Point> points = { Point(0,0), Point(100,0), Point(100,100), Point(0,100) };

    auto hull = jarvis.findConvexHull(points);

    // ASSERTION
    EXPECT_EQ(hull.size(), 4);

    // MATCH
    EXPECT_THAT(hull, testing::UnorderedElementsAre(
        Point(0, 0), Point(100, 0), Point(100, 100), Point(0, 100)
    ));
}

class MockDisplay {
public:
    MOCK_METHOD(void, drawPoints, (const std::vector<Point>&), ());
    MOCK_METHOD(void, drawHull, (const std::vector<Point>&), ());
    MOCK_METHOD(void, showMessage, (const std::string&), ());
};

TEST(JarvisAlgorithmWithDisplayTest, JR07WithMock)
{
    MockDisplay mockDisplay;

    EXPECT_CALL(mockDisplay, drawPoints(testing::_)).Times(1);
    EXPECT_CALL(mockDisplay, drawHull(testing::_)).Times(1);

    std::vector<Point> points = { Point(0,0), Point(100,0), Point(100,100), Point(0,100) };

    JarvisAlgorithm jarvis;
    auto hull = jarvis.findConvexHull(points);

    mockDisplay.drawPoints(points);
    mockDisplay.drawHull(hull);

    EXPECT_EQ(hull.size(), 4);
}