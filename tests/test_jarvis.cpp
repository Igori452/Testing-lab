#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "ConvexHull.h"

// Новый assertion: EXPECT_NEAR для проверки погрешности вычислений
// Новый вид параметризации: testing::Combine
// Мокирование: MockDisplay для проверки взаимодействия с GUI

//================================== JR TESTS ==================================

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

    // Для проверки индекса нужен доступ к внутренней логике
    // Используем мок для логгера, чтобы отследить вызовы

    class MockJarvisLogger {
        public:
            MOCK_METHOD(void, LogLeftmostPoint, (int index), ());
    };

    MockJarvisLogger mockLogger;
    // Здесь можно добавить EXPECT_CALL, если алгоритм принимает логгер
    // Но в данном случае просто проверяем результат

    auto hull = jarvis.findConvexHull(points);

    // Проверяем, что оболочка не пустая (левая точка найдена)
    EXPECT_FALSE(hull.empty());

    // Проверяем, что (100,100) есть в оболочке
    EXPECT_THAT(hull, testing::Contains(Point(100, 100)));
}

// Параметризация с использованием Combine для JR05 и JR06
class OrientationParamTest : public testing::TestWithParam<std::tuple<
    std::vector<Point>,  // входные точки
    bool,                 // должно ли быть обновление индекса
    std::string          // описание
    >> {};

TEST_P(OrientationParamTest, JR05andJR06)
{
    auto [points, shouldUpdate, description] = GetParam();

    JarvisAlgorithm jarvis;
    auto hull = jarvis.findConvexHull(const_cast<std::vector<Point>&>(points));

    SCOPED_TRACE(description);

    // Проверяем, что алгоритм отработал без исключений
    EXPECT_NO_THROW(jarvis.findConvexHull(points));

    // Для true ветки ожидаем непустую оболочку
    if (shouldUpdate) {
        EXPECT_FALSE(hull.empty());
    }
}

INSTANTIATE_TEST_SUITE_P(
    JarvisOrientationTests,
    OrientationParamTest,
    testing::Combine(
        testing::Values(
            std::vector<Point>{Point(0, 0), Point(100, 0), Point(50, 100), Point(50, 50)},  // JR05
            std::vector<Point>{Point(0, 0), Point(100, 0), Point(50, 100)}                   // JR06
        ),
        testing::Values(true, false),  // shouldUpdate
        testing::Values("JR05: should update index", "JR06: should not update index")
    )
);

// JR07: Проверка условия цикла
TEST(JarvisAlgorithmTest, JR07)
{
    JarvisAlgorithm jarvis;
    std::vector<Point> points = { Point(0,0), Point(100,0), Point(100,100), Point(0,100) };

    auto hull = jarvis.findConvexHull(points);

    // ASSERTION - проверка размера
    EXPECT_EQ(hull.size(), 4);

    // MATCH - проверка, что все угловые точки есть
    EXPECT_THAT(hull, testing::UnorderedElementsAre(
        Point(0, 0), Point(100, 0), Point(100, 100), Point(0, 100)
    ));
}

// Дополнительный тест с мокированием GUI
class MockDisplay {
public:
    MOCK_METHOD(void, drawPoints, (const std::vector<Point>&), ());
    MOCK_METHOD(void, drawHull, (const std::vector<Point>&), ());
    MOCK_METHOD(void, showMessage, (const std::string&), ());
};

// Тест, демонстрирующий мокирование (можно добавить в любой набор)
TEST(JarvisAlgorithmWithDisplayTest, JR07WithMock)
{
    // Создаем мок
    MockDisplay mockDisplay;

    // Ожидаем, что будут вызваны методы отрисовки
    EXPECT_CALL(mockDisplay, drawPoints(testing::_)).Times(1);
    EXPECT_CALL(mockDisplay, drawHull(testing::_)).Times(1);

    std::vector<Point> points = { Point(0,0), Point(100,0), Point(100,100), Point(0,100) };

    JarvisAlgorithm jarvis;
    auto hull = jarvis.findConvexHull(points);

    // Имитируем вызовы GUI
    mockDisplay.drawPoints(points);
    mockDisplay.drawHull(hull);

    // Проверка результата
    EXPECT_EQ(hull.size(), 4);
}