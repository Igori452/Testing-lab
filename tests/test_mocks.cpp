#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "ConvexHull.h"

using ::testing::_;
using ::testing::Return;
using ::testing::AtLeast;

// ==================== МОК ДЛЯ ЛОГГЕРА ====================
class MockLogger {
public:
    MOCK_METHOD(void, log, (const std::string& message));
};

// Модифицированный алгоритм с логгером
class JarvisAlgorithmWithLogger : public JarvisAlgorithm {
private:
    MockLogger* logger;
public:
    JarvisAlgorithmWithLogger(MockLogger* l) : logger(l) {}

    std::vector<Point> findConvexHull(std::vector<Point>& points) {
        logger->log("The beginning of the Jarvis algorithm");
        auto result = JarvisAlgorithm::findConvexHull(points);
        logger->log("The end of Jarvis's algorithm");
        return result;
    }
};

// ==================== МОК ДЛЯ ВАЛИДАТОРА ====================
class MockValidator {
public:
    MOCK_METHOD(bool, isValidPoint, (const Point& p));
};

class GrahamAlgorithmWithValidator : public GrahamAlgorithm {
private:
    MockValidator* validator;
public:
    GrahamAlgorithmWithValidator(MockValidator* v) : validator(v) {}

    std::vector<Point> findConvexHull(std::vector<Point>& points) {
        std::vector<Point> validPoints;
        for (const auto& p : points) {
            if (validator->isValidPoint(p)) {
                validPoints.push_back(p);
            }
        }
        return GrahamAlgorithm::findConvexHull(validPoints);
    }
};

// ==================== МОК ДЛЯ ОБРАБОТЧИКА РЕЗУЛЬТАТОВ ====================
class MockResultHandler {
public:
    MOCK_METHOD(void, handleResult, (const std::vector<Point>& hull));
};

class AlgorithmWithResultHandler : public JarvisAlgorithm {
private:
    MockResultHandler* handler;
public:
    AlgorithmWithResultHandler(MockResultHandler* h) : handler(h) {}

    std::vector<Point> findConvexHull(std::vector<Point>& points) {
        auto result = JarvisAlgorithm::findConvexHull(points);
        handler->handleResult(result);
        return result;
    }
};

// ==================== ТЕСТЫ ====================

// Тест 1: Проверка логгера с алгоритмом Джарвиса
TEST(MockTests, JarvisWithLogger)
{
    MockLogger mockLogger;
    JarvisAlgorithmWithLogger jarvis(&mockLogger);

    EXPECT_CALL(mockLogger, log("The beginning of the Jarvis algorithm")).Times(1);
    EXPECT_CALL(mockLogger, log("The end of Jarvis's algorithm")).Times(1);

    std::vector<Point> points = { Point(0,0), Point(100,0), Point(0,100) };
    auto hull = jarvis.findConvexHull(points);

    EXPECT_EQ(hull.size(), 3);
}

// Тест 2: Проверка валидатора с алгоритмом Грэхема
TEST(MockTests, GrahamWithValidator)
{
    MockValidator mockValidator;
    GrahamAlgorithmWithValidator graham(&mockValidator);

    EXPECT_CALL(mockValidator, isValidPoint(_))
        .Times(3)
        .WillOnce(Return(true))
        .WillOnce(Return(true))
        .WillOnce(Return(true));

    std::vector<Point> points = { Point(0,0), Point(100,0), Point(0,100) };
    auto hull = graham.findConvexHull(points);

    EXPECT_EQ(hull.size(), 3);
}

// Тест 3: Проверка обработчика результатов
TEST(MockTests, AlgorithmWithResultHandler)
{
    MockResultHandler mockHandler;
    AlgorithmWithResultHandler algo(&mockHandler);

    std::vector<Point> points = { Point(0,0), Point(100,0), Point(0,100) };

    EXPECT_CALL(mockHandler, handleResult(_))
        .Times(1);

    auto hull = algo.findConvexHull(points);

    EXPECT_EQ(hull.size(), 3);
}