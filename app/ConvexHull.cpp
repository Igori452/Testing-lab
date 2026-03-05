#include "ConvexHull.h"
#include <algorithm>
#include <cmath>

// Оператор сравнения для использования в std::find и EXPECT_EQ
bool Point::operator==(const Point& other) const {
    const double eps = 1e-8; // погрешность для double
    return std::abs(x - other.x) < eps && std::abs(y - other.y) < eps;
}

// Оператор неравенства (хороший тон добавлять)
bool Point::operator!=(const Point& other) const {
    return !(*this == other);
}

std::vector<Point> JarvisAlgorithm::findConvexHull(std::vector<Point>& points)
{
    std::vector<Point> hull;

    if (points.size() < 3)
        return hull; // less than 3 points -> hull impossible

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

    // Filter points by renderable coordinates
    std::vector<Point> filtered;
    for (const auto& p : points) {
        if (p.x >= 0 && p.x <= 600 && p.y >= 0 && p.y <= 600)
            filtered.push_back(p);
    }
    points = filtered;
    
    if (points.size() < 3)
        return {};

    // Check if all points are collinear
    Point p0 = points[0];
    Point p1 = points[1];
    bool allCollinear = true;
    for (size_t i = 2; i < points.size(); ++i)
    {
        Point pi = points[i];
        double cross = (p1.y - p0.y) * (pi.x - p0.x) - (p1.x - p0.x) * (pi.y - p0.y);
        if (std::abs(cross) > 1e-8) // small epsilon for double
        {
            allCollinear = false;
            break;
        }
    }

    if (allCollinear) return {}; // empty hull

    // Jarvis algorithm - ИСПРАВЛЕННАЯ ВЕРСИЯ
    // Находим самую левую нижнюю точку (не только по x)
    size_t leftmost = 0;
    for (size_t i = 1; i < points.size(); ++i) {
        if (points[i].x < points[leftmost].x - 1e-8) {
            leftmost = i;
        } else if (std::abs(points[i].x - points[leftmost].x) < 1e-8) {
            if (points[i].y < points[leftmost].y) {
                leftmost = i;
            }
        }
    }

    size_t p = leftmost;
    do {
        hull.push_back(points[p]);
        
        // Ищем точку q, которая дает наименьший полярный угол относительно p
        size_t q = 0;
        if (p == 0) q = 1; else q = 0; // Начинаем с любой другой точки
        
        for (size_t i = 0; i < points.size(); ++i) {
            if (i == p) continue; // Пропускаем текущую точку
            
            // Вычисляем ориентацию тройки (p, q, i)
            double cross = (points[q].x - points[p].x) * (points[i].y - points[p].y) -
                          (points[q].y - points[p].y) * (points[i].x - points[p].x);
            
            // Защита от NaN и бесконечности
            if (std::isnan(cross) || std::isinf(cross))
                continue;
            
            // Если cross > 0, то i находится слева от вектора p->q (против часовой)
            // Если cross < 0, то i находится справа от вектора p->q (по часовой)
            // Нам нужен максимальный поворот против часовой стрелки (наименьший полярный угол)
            if (cross > 1e-8) {
                // i дает поворот против часовой стрелки - это лучше
                q = i;
            } else if (std::abs(cross) < 1e-8) {
                // Точки коллинеарны - выбираем самую дальнюю от p
                double dist_q = (points[q].x - points[p].x) * (points[q].x - points[p].x) +
                               (points[q].y - points[p].y) * (points[q].y - points[p].y);
                double dist_i = (points[i].x - points[p].x) * (points[i].x - points[p].x) +
                               (points[i].y - points[p].y) * (points[i].y - points[p].y);
                if (dist_i > dist_q) {
                    q = i;
                }
            }
            // Если cross < 0, i дает поворот по часовой - пропускаем
        }
        
        p = q;
        
        // Защита от зацикливания
        if (hull.size() > points.size()) {
            return {};
        }
        
    } while (p != leftmost);

    // Удаляем коллинеарные точки из оболочки (оставляем только вершины)
    std::vector<Point> simplified_hull;
    for (size_t i = 0; i < hull.size(); ++i) {
        size_t prev = (i + hull.size() - 1) % hull.size();
        size_t next = (i + 1) % hull.size();
        
        double cross = (hull[next].x - hull[i].x) * (hull[prev].y - hull[i].y) -
                      (hull[next].y - hull[i].y) * (hull[prev].x - hull[i].x);
        
        // Если не коллинеарна с соседями, оставляем
        if (std::abs(cross) > 1e-8) {
            simplified_hull.push_back(hull[i]);
        }
    }
    
    // Если после упрощения осталось меньше 3 точек, возвращаем исходную оболочку
    if (simplified_hull.size() < 3) {
        return hull;
    }
    
    return simplified_hull;
}

std::vector<Point> GrahamAlgorithm::findConvexHull(std::vector<Point>& points)
{
    std::vector<Point> hull;

    if (points.size() < 3)
        return hull; // less than 3 points -> hull impossible

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

    // Filter points by renderable coordinates
    std::vector<Point> filtered;
    for (const auto& p : points) {
        if (p.x >= 0 && p.x <= 600 && p.y >= 0 && p.y <= 600)
            filtered.push_back(p);
    }
    points = filtered;
    
    if (points.size() < 3)
        return {};

    // Check if all points are collinear
    Point p0 = points[0];
    Point p1 = points[1];
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

    if (allCollinear)
        return {}; // empty hull

    // ИСПРАВЛЕНО: Find point with lowest y (and leftmost x if y equal)
    size_t lowest = 0;
    for (size_t i = 1; i < points.size(); ++i)
    {
        // Используем эпсилон для сравнения double
        if (points[i].y < points[lowest].y - 1e-8) {
            lowest = i;
        }
        else if (std::abs(points[i].y - points[lowest].y) < 1e-8) {
            // Если Y равны (с учетом эпсилон), выбираем по X
            if (points[i].x < points[lowest].x - 1e-8) {
                lowest = i;
            }
        }
    }

    // Swap starting point with first element
    std::swap(points[0], points[lowest]);

    // Sort points by polar angle relative to start point
    Point start = points[0];
    std::sort(points.begin() + 1, points.end(),
        [&start](const Point& a, const Point& b) {
            double cross = (a.y - start.y) * (b.x - start.x) -
                (a.x - start.x) * (b.y - start.y);
            
            // Защита от NaN и бесконечности
            if (std::isnan(cross) || std::isinf(cross))
                return false;
            
            if (std::abs(cross) < 1e-8) // Collinear
            {
                double distA = (a.x - start.x) * (a.x - start.x) +
                    (a.y - start.y) * (a.y - start.y);
                double distB = (b.x - start.x) * (b.x - start.x) +
                    (b.y - start.y) * (b.y - start.y);
                return distA < distB;
            }
            return cross > 0; // counter-clockwise order
        });

    // ИСПРАВЛЕНО: Remove duplicate points and collinear points
    std::vector<Point> uniquePoints;
    uniquePoints.push_back(points[0]);
    
    for (size_t i = 1; i < points.size(); ++i) {
        // Проверяем, не дубликат ли это
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

    // Build convex hull using stack
    if (uniquePoints.size() < 2) return {};
    
    std::vector<Point> stack;
    stack.push_back(uniquePoints[0]);
    stack.push_back(uniquePoints[1]);

    for (size_t i = 2; i < uniquePoints.size(); ++i) {
        while (stack.size() >= 2) {
            Point top = stack.back();
            Point second = stack[stack.size() - 2];

            // Calculate orientation (second -> top -> uniquePoints[i])
            double cross = (top.y - second.y) * (uniquePoints[i].x - top.x) -
                (top.x - second.x) * (uniquePoints[i].y - top.y);
            
            // Защита от NaN и бесконечности
            if (std::isnan(cross) || std::isinf(cross))
                break;

            if (cross <= 0) // right turn or collinear
                stack.pop_back(); // remove point from stack
            else break; // left turn - good
        }
        stack.push_back(uniquePoints[i]);
    }

    return stack;
}