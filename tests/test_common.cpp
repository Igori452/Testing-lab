#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "ConvexHull.h"

// Реализация общих тестов черного ящика для обоих алгоритмов (6 assertions and 3 matchers)

//================================== EP ==================================

TEST(EpTestsForAlgorithms, EP01) 
{
	JarvisAlgorithm jarvis;
	GrahamAlgorithm graham;

	std::vector<Point> points = {};

	auto jarvisResult = jarvis.findConvexHull(points);
	auto grahamResult = graham.findConvexHull(points);

	// ASSERTION
	EXPECT_TRUE(jarvisResult.empty());
	EXPECT_TRUE(grahamResult.empty());

	EXPECT_EQ(jarvisResult.size(), 0);
	EXPECT_EQ(grahamResult.size(), 0);
}

TEST(EpTestsForAlgorithms, EP02)
{
	JarvisAlgorithm jarvis;
	GrahamAlgorithm graham;

	std::vector<Point> points = { Point(100, 100), Point(100, 100), Point(100, 100), Point(100, 100) };

	auto jarvisResult = jarvis.findConvexHull(points);
	auto grahamResult = graham.findConvexHull(points);

	// ASSERTION
	EXPECT_TRUE(jarvisResult.empty());
	EXPECT_TRUE(grahamResult.empty());

	EXPECT_EQ(jarvisResult.size(), 0);
	EXPECT_EQ(grahamResult.size(), 0);
}

TEST(EpTestsForAlgorithms, EP03)
{
	JarvisAlgorithm jarvis;
	GrahamAlgorithm graham;

	std::vector<Point> points = {Point(100, 100), Point(200, 200)};

	auto jarvisResult = jarvis.findConvexHull(points);
	auto grahamResult = graham.findConvexHull(points);

	// ASSERTION
	EXPECT_TRUE(jarvisResult.empty());
	EXPECT_TRUE(grahamResult.empty());

	EXPECT_EQ(jarvisResult.size(), 0);
	EXPECT_EQ(grahamResult.size(), 0);
}

TEST(EpTestsForAlgorithms, EP04)
{
	JarvisAlgorithm jarvis;
	GrahamAlgorithm graham;

	std::vector<Point> points = { Point(0, 0), Point(100, 0), Point(0, 100) };

	auto jarvisResult = jarvis.findConvexHull(points);
	auto grahamResult = graham.findConvexHull(points);

	// ASSERTION
	EXPECT_FALSE(jarvisResult.empty());
	EXPECT_FALSE(grahamResult.empty());

	//MATCH
	for (const auto& p : points) 
	{
		EXPECT_THAT(jarvisResult, testing::Contains(p));
		EXPECT_THAT(grahamResult, testing::Contains(p));
	}
}

TEST(EpTestsForAlgorithms, EP05)
{
	JarvisAlgorithm jarvis;
	GrahamAlgorithm graham;

	std::vector<Point> points = { Point(0, 0), Point(50, 50), Point(100, 100) };

	auto jarvisResult = jarvis.findConvexHull(points);
	auto grahamResult = graham.findConvexHull(points);

	// ASSERTION
	EXPECT_TRUE(jarvisResult.empty());
	EXPECT_TRUE(grahamResult.empty());

	EXPECT_EQ(jarvisResult.size(), 0);
	EXPECT_EQ(grahamResult.size(), 0);
}

TEST(EpTestsForAlgorithms, EP06)
{
	JarvisAlgorithm jarvis;
	GrahamAlgorithm graham;

	std::vector<Point> points = { Point(0, 0), Point(100, 0), Point(100, 100), 
								  Point(0, 100), Point(50, 50) 
								};

	auto jarvisResult = jarvis.findConvexHull(points);
	auto grahamResult = graham.findConvexHull(points);

	// ASSERTION
	EXPECT_EQ(jarvisResult.size(), 4);
	EXPECT_EQ(grahamResult.size(), 4);

	//MATCH
	EXPECT_THAT(jarvisResult, testing::Each(testing::Ne(Point(50, 50))));
	EXPECT_THAT(grahamResult, testing::Each(testing::Ne(Point(50, 50))));
}

//================================== BV ==================================

TEST(BvTestsForAlgorithms, BV01)
{
	JarvisAlgorithm jarvis;
	GrahamAlgorithm graham;

	std::vector<Point> points = { Point(100, 100) };

	auto jarvisResult = jarvis.findConvexHull(points);
	auto grahamResult = graham.findConvexHull(points);

	// ASSERTION
	EXPECT_TRUE(jarvisResult.empty());
	EXPECT_TRUE(grahamResult.empty());

	EXPECT_EQ(jarvisResult.size(), 0);
	EXPECT_EQ(grahamResult.size(), 0);
}

TEST(BvTestsForAlgorithms, BV02)
{
	JarvisAlgorithm jarvis;
	GrahamAlgorithm graham;

	std::vector<Point> points = { Point(100, 100), Point(200, 200) };

	auto jarvisResult = jarvis.findConvexHull(points);
	auto grahamResult = graham.findConvexHull(points);

	// ASSERTION
	EXPECT_TRUE(jarvisResult.empty());
	EXPECT_TRUE(grahamResult.empty());

	EXPECT_EQ(jarvisResult.size(), 0);
	EXPECT_EQ(grahamResult.size(), 0);
}

// Parameterization tests

struct ThreePointsTestCase {
	std::vector<Point> points;
	bool colinear;
};

class ThreePointsTest : public testing::TestWithParam<ThreePointsTestCase> {};

TEST_P(ThreePointsTest, BV03andBV04)
{
	JarvisAlgorithm jarvis;
	GrahamAlgorithm graham;

	ThreePointsTestCase param = GetParam();

	auto jarvisResult = jarvis.findConvexHull(param.points);
	auto grahamResult = graham.findConvexHull(param.points);

	if (!param.colinear) 
	{
		// ASSERTION
		EXPECT_FALSE(jarvisResult.empty());
		EXPECT_FALSE(grahamResult.empty());

		//MATCH
		for (const auto& p : param.points)
		{
			EXPECT_THAT(jarvisResult, testing::Contains(p));
			EXPECT_THAT(grahamResult, testing::Contains(p));
		}
	}
	else
	{
		// ASSERTION
		EXPECT_TRUE(jarvisResult.empty());
		EXPECT_TRUE(grahamResult.empty());

		EXPECT_EQ(jarvisResult.size(), 0);
		EXPECT_EQ(grahamResult.size(), 0);
	}
}

INSTANTIATE_TEST_SUITE_P(
	BvTestsForAlgorithms,
	ThreePointsTest,
	testing::Values(
		ThreePointsTestCase{ {Point(0, 0), Point(100, 0), Point(0, 100)}, false },
		ThreePointsTestCase{ {Point(0, 0), Point(50, 50), Point(100, 100)}, true }
	)
);


TEST(BvTestsForAlgorithms, BV05)
{
	GTEST_SKIP() << "The test has been disabled because the developer is aware of the error.";

	JarvisAlgorithm jarvis;
	GrahamAlgorithm graham;

	std::vector<Point> points = { Point(0, 0), Point(100, 0), Point(100, 100),
								  Point(50, 50)
	};

	auto jarvisResult = jarvis.findConvexHull(points);
	auto grahamResult = graham.findConvexHull(points);

	// ASSERTION
	EXPECT_EQ(jarvisResult.size(), 3);
	EXPECT_EQ(grahamResult.size(), 3);
}

TEST(BvTestsForAlgorithms, BV06)
{
	JarvisAlgorithm jarvis;
	GrahamAlgorithm graham;

	std::vector<Point> points = { Point(0, 0), 
								  Point(50, 50), 
								  Point(100, 100),
								  Point(150, 150),
								  Point(150, 50)
								};

	auto jarvisResult = jarvis.findConvexHull(points);
	auto grahamResult = graham.findConvexHull(points);

	// ASSERTION
	EXPECT_EQ(jarvisResult.size(), 3);
	EXPECT_EQ(grahamResult.size(), 3);

	// MATCH
	EXPECT_THAT(jarvisResult, testing::UnorderedElementsAre(
		Point(0, 0), Point(150, 150), Point(150, 50)
	));

	EXPECT_THAT(grahamResult, testing::UnorderedElementsAre(
		Point(0, 0), Point(150, 150), Point(150, 50)
	));
}

bool hasValidHullForDuplicates(const std::vector<Point>& hull) {
	return hull.empty();  // для дубликатов должна быть пустая оболочка
}

TEST(BvTestsForAlgorithms, BV07)
{
	std::vector<Point> points = {
		Point(100,100), Point(100,100), Point(100,100), Point(200,200)
	};

	JarvisAlgorithm jarvis;
	GrahamAlgorithm graham;

	auto jarvisResult = jarvis.findConvexHull(points);
	auto grahamResult = graham.findConvexHull(points);


	// ASSERTION
	EXPECT_PRED1(hasValidHullForDuplicates, jarvisResult);
	EXPECT_PRED1(hasValidHullForDuplicates, grahamResult);
}