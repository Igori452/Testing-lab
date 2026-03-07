#include "ConvexHull.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <chrono>

// Реализация оператора сравнения
bool Point::operator==(const Point& other) const {
    const double eps = 1e-8;
    return std::abs(x - other.x) < eps && std::abs(y - other.y) < eps;
}

// Оператор неравенства (если нужен)
bool Point::operator!=(const Point& other) const {
    return !(*this == other);
}

// =======================================================
// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ДЛЯ ПРОФИЛИРОВАНИЯ
// =======================================================

struct ProfileData {
    double filter_ms = 0;
    double collinear_ms = 0;
    double search_ms = 0;
    double sort_ms = 0;
    double unique_ms = 0;
    double stack_ms = 0;
    double jarvis_loop_ms = 0;
    double simplify_ms = 0;
    size_t point_count = 0;
};

void printProfile(const std::string& algo, const ProfileData& data) {
    std::cout << "\n=== " << algo << " PROFILING ===\n";
    std::cout << "Points: " << data.point_count << "\n";
    if (data.filter_ms > 0) std::cout << "Filter: " << data.filter_ms << " ms\n";
    if (data.collinear_ms > 0) std::cout << "Collinear check: " << data.collinear_ms << " ms\n";
    if (data.search_ms > 0) std::cout << "Search point: " << data.search_ms << " ms\n";
    if (data.sort_ms > 0) std::cout << "Sort: " << data.sort_ms << " ms\n";
    if (data.unique_ms > 0) std::cout << "Unique points: " << data.unique_ms << " ms\n";
    if (data.stack_ms > 0) std::cout << "Stack build: " << data.stack_ms << " ms\n";
    if (data.jarvis_loop_ms > 0) std::cout << "Jarvis loop: " << data.jarvis_loop_ms << " ms\n";
    if (data.simplify_ms > 0) std::cout << "Simplify: " << data.simplify_ms << " ms\n";

    double total = data.filter_ms + data.collinear_ms + data.search_ms +
        data.sort_ms + data.unique_ms + data.stack_ms +
        data.jarvis_loop_ms + data.simplify_ms;
    std::cout << "TOTAL: " << total << " ms\n";
}

// =======================================================
// JARVIS ALGORITHM
// =======================================================

std::vector<Point> JarvisAlgorithm::findConvexHull(std::vector<Point>& points)
{
    ProfileData profile; // for profiling
    profile.point_count = points.size(); // for profiling

    auto func_start = std::chrono::high_resolution_clock::now(); // for profiling
    std::vector<Point> hull;

    if (points.size() < 3)
        return hull;

    // Проверка на одинаковые точки
    bool allSame = true;
    for (size_t i = 1; i < points.size(); ++i) {
        if (std::abs(points[i].x - points[0].x) > 1e-8 ||
            std::abs(points[i].y - points[0].y) > 1e-8) {
            allSame = false;
            break;
        }
    }
    if (allSame) return {};

    // Фильтрация
    auto filter_start = std::chrono::high_resolution_clock::now(); // for profiling
    std::vector<Point> filtered;
    for (const auto& p : points) {
        if (p.x >= 0 && p.x <= 600 && p.y >= 0 && p.y <= 600)
            filtered.push_back(p);
    }
    points = filtered;
    auto filter_end = std::chrono::high_resolution_clock::now(); // for profiling
    profile.filter_ms = std::chrono::duration<double, std::milli>(filter_end - filter_start).count(); // for profiling

    if (points.size() < 3)
        return {};

    // Проверка коллинеарности
    auto collinear_start = std::chrono::high_resolution_clock::now(); // for profiling
    Point p0 = points[0];
    Point p1;

    size_t j = 1;
    while (j < points.size() && points[j] == p0)
        j++;

    if (j == points.size())
        return {};

    p1 = points[j];
    bool allCollinear = true;
    for (size_t i = 2; i < points.size(); ++i)
    {
        Point pi = points[i];
        double cross = (p1.y - p0.y) * (pi.x - p0.x) - (p1.x - p0.x) * (pi.y - p0.y);
        if (std::abs(cross) > 1e-8)
        {
            allCollinear = false;
            break;
        }
    }
    auto collinear_end = std::chrono::high_resolution_clock::now(); // for profiling
    profile.collinear_ms = std::chrono::duration<double, std::milli>(collinear_end - collinear_start).count(); // for profiling

    if (allCollinear) return {};

    // Поиск самой левой нижней точки
    auto search_start = std::chrono::high_resolution_clock::now(); // for profiling
    size_t leftmost = 0;
    for (size_t i = 1; i < points.size(); ++i) {
        if (points[i].x < points[leftmost].x - 1e-8) {
            leftmost = i;
        }
        else if (std::abs(points[i].x - points[leftmost].x) < 1e-8) {
            if (points[i].y < points[leftmost].y) {
                leftmost = i;
            }
        }
    }
    auto search_end = std::chrono::high_resolution_clock::now(); // for profiling
    profile.search_ms = std::chrono::duration<double, std::milli>(search_end - search_start).count(); // for profiling

    // Основной цикл Джарвиса
    auto jarvis_start = std::chrono::high_resolution_clock::now(); // for profiling
    size_t p = leftmost;
    do {
        hull.push_back(points[p]);

        size_t q = 0;
        if (p == 0) q = 1; else q = 0;

        for (size_t i = 0; i < points.size(); ++i) {
            if (i == p) continue;

            double cross = (points[q].x - points[p].x) * (points[i].y - points[p].y) -
                (points[q].y - points[p].y) * (points[i].x - points[p].x);

            if (std::isnan(cross) || std::isinf(cross))
                continue;

            if (cross > 1e-8) {
                q = i;
            }
            else if (std::abs(cross) < 1e-8) {
                double dist_q = (points[q].x - points[p].x) * (points[q].x - points[p].x) +
                    (points[q].y - points[p].y) * (points[q].y - points[p].y);
                double dist_i = (points[i].x - points[p].x) * (points[i].x - points[p].x) +
                    (points[i].y - points[p].y) * (points[i].y - points[p].y);
                if (dist_i > dist_q) {
                    q = i;
                }
            }
        }

        p = q;

        if (hull.size() > points.size()) {
            return {};
        }

    } while (p != leftmost);
    auto jarvis_end = std::chrono::high_resolution_clock::now(); // for profiling
    profile.jarvis_loop_ms = std::chrono::duration<double, std::milli>(jarvis_end - jarvis_start).count(); // for profiling

    // Упрощение оболочки
    auto simplify_start = std::chrono::high_resolution_clock::now(); // for profiling
    std::vector<Point> simplified_hull;
    for (size_t i = 0; i < hull.size(); ++i) {
        size_t prev = (i + hull.size() - 1) % hull.size();
        size_t next = (i + 1) % hull.size();

        double cross = (hull[next].x - hull[i].x) * (hull[prev].y - hull[i].y) -
            (hull[next].y - hull[i].y) * (hull[prev].x - hull[i].x);

        if (std::abs(cross) > 1e-8) {
            simplified_hull.push_back(hull[i]);
        }
    }
    auto simplify_end = std::chrono::high_resolution_clock::now(); // for profiling 
    profile.simplify_ms = std::chrono::duration<double, std::milli>(simplify_end - simplify_start).count(); // for profiling

    auto func_end = std::chrono::high_resolution_clock::now(); // for profiling
    double total = std::chrono::duration<double, std::milli>(func_end - func_start).count(); // for profiling

    printProfile("JARVIS", profile); // for profiling
    std::cout << "Function TOTAL: " << total << " ms\n"; // for profiling

    if (simplified_hull.size() < 3) {
        return hull;
    }

    return simplified_hull;
}

// =======================================================
// GRAHAM ALGORITHM
// =======================================================

std::vector<Point> GrahamAlgorithm::findConvexHull(std::vector<Point>& points)
{
    ProfileData profile; // for profiling
    profile.point_count = points.size(); // for profiling

    auto func_start = std::chrono::high_resolution_clock::now(); // for profiling
    std::vector<Point> hull;

    if (points.size() < 3)
        return hull;

    // Проверка на одинаковые точки
    bool allSame = true;
    for (size_t i = 1; i < points.size(); ++i) {
        if (std::abs(points[i].x - points[0].x) > 1e-8 ||
            std::abs(points[i].y - points[0].y) > 1e-8) {
            allSame = false;
            break;
        }
    }
    if (allSame) return {};

    // Фильтрация
    auto filter_start = std::chrono::high_resolution_clock::now(); // for profiling
    std::vector<Point> filtered;
    for (const auto& p : points) {
        if (p.x >= 0 && p.x <= 600 && p.y >= 0 && p.y <= 600)
            filtered.push_back(p);
    }
    points = filtered;
    auto filter_end = std::chrono::high_resolution_clock::now(); // for profiling 
    profile.filter_ms = std::chrono::duration<double, std::milli>(filter_end - filter_start).count(); // for profiling

    if (points.size() < 3)
        return {};

    // Проверка коллинеарности
    auto collinear_start = std::chrono::high_resolution_clock::now(); // for profiling
    Point p0 = points[0];
    Point p1;

    size_t j = 1;
    while (j < points.size() && points[j] == p0)
        j++;

    if (j == points.size())
        return {};

    p1 = points[j];
    bool allCollinear = true;

    for (size_t i = 2; i < points.size(); ++i)
    {
        Point pi = points[i];
        double cross = (p1.y - p0.y) * (pi.x - p0.x) - (p1.x - p0.x) * (pi.y - p0.y);
        if (std::abs(cross) > 1e-8)
        {
            allCollinear = false;
            break;
        }
    }
    auto collinear_end = std::chrono::high_resolution_clock::now(); // for profiling
    profile.collinear_ms = std::chrono::duration<double, std::milli>(collinear_end - collinear_start).count(); // for profiling

    if (allCollinear)
        return {};

    // Поиск самой нижней точки
    auto search_start = std::chrono::high_resolution_clock::now(); // for profiling
    size_t lowest = 0;
    for (size_t i = 1; i < points.size(); ++i)
    {
        if (points[i].y < points[lowest].y - 1e-8) {
            lowest = i;
        }
        else if (std::abs(points[i].y - points[lowest].y) < 1e-8) {
            if (points[i].x < points[lowest].x - 1e-8) {
                lowest = i;
            }
        }
    }
    auto search_end = std::chrono::high_resolution_clock::now(); // for profiling
    profile.search_ms = std::chrono::duration<double, std::milli>(search_end - search_start).count(); // for profiling

    std::swap(points[0], points[lowest]);
    Point start = points[0];

    // Сортировка
    auto sort_start = std::chrono::high_resolution_clock::now(); // for profiling
    std::sort(points.begin() + 1, points.end(),
        [&start](const Point& a, const Point& b) {
            double cross = (a.y - start.y) * (b.x - start.x) -
                (a.x - start.x) * (b.y - start.y);

            if (std::isnan(cross) || std::isinf(cross))
                return false;

            if (std::abs(cross) < 1e-8)
            {
                double distA = (a.x - start.x) * (a.x - start.x) +
                    (a.y - start.y) * (a.y - start.y);
                double distB = (b.x - start.x) * (b.x - start.x) +
                    (b.y - start.y) * (b.y - start.y);
                return distA < distB;
            }
            return cross > 0;
        });
    auto sort_end = std::chrono::high_resolution_clock::now(); // for profiling
    profile.sort_ms = std::chrono::duration<double, std::milli>(sort_end - sort_start).count(); // for profiling

    // Удаление дубликатов
    auto unique_start = std::chrono::high_resolution_clock::now(); // for profiling
    std::vector<Point> uniquePoints;
    uniquePoints.push_back(points[0]);

    for (size_t i = 1; i < points.size(); ++i) {
        bool isDuplicate = false;
        for (const auto& p : uniquePoints) {
            if (std::abs(p.x - points[i].x) < 1e-8 &&
                std::abs(p.y - points[i].y) < 1e-8) {
                isDuplicate = true;
                break;
            }
        }
        if (!isDuplicate) {
            uniquePoints.push_back(points[i]);
        }
    } 
    auto unique_end = std::chrono::high_resolution_clock::now(); // for profiling
    profile.unique_ms = std::chrono::duration<double, std::milli>(unique_end - unique_start).count(); // for profiling

    if (uniquePoints.size() < 2) return {};

    // Построение стека
    auto stack_start = std::chrono::high_resolution_clock::now(); // for profiling
    std::vector<Point> stack;
    stack.push_back(uniquePoints[0]);
    stack.push_back(uniquePoints[1]);

    for (size_t i = 2; i < uniquePoints.size(); ++i) {
        while (stack.size() >= 2) {
            Point top = stack.back();
            Point second = stack[stack.size() - 2];

            double cross = (top.y - second.y) * (uniquePoints[i].x - top.x) -
                (top.x - second.x) * (uniquePoints[i].y - top.y);

            if (std::isnan(cross) || std::isinf(cross))
                break;

            if (cross <= 0)
                stack.pop_back();
            else break;
        }
        stack.push_back(uniquePoints[i]);
    } 
    auto stack_end = std::chrono::high_resolution_clock::now(); // for profiling
    profile.stack_ms = std::chrono::duration<double, std::milli>(stack_end - stack_start).count(); // for profiling

    auto func_end = std::chrono::high_resolution_clock::now(); // for profiling
    double total = std::chrono::duration<double, std::milli>(func_end - func_start).count(); // for profiling
     
    printProfile("GRAHAM", profile); // for profiling
    std::cout << "Function TOTAL: " << total << " ms\n"; // for profiling

    return stack;
}