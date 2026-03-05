#include "ConvexHull.h"
#include <algorithm>

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

    // Filter points by renderable coordinates
    std::vector<Point> filtered;
    for (const auto& p : points) {
        if (p.x >= 0 && p.x <= 600 && p.y >= 0 && p.y <= 600)
            filtered.push_back(p);
    }
    points = filtered;

    // Jarvis algorithm
    size_t leftmost = 0;
    for (size_t i = 1; i < points.size(); ++i)
        if (points[i].x < points[leftmost].x)
            leftmost = i;

    size_t p = leftmost;
    do {
        hull.push_back(points[p]);
        size_t q = (p + 1) % points.size();

        for (size_t i = 0; i < points.size(); ++i)
        {
            double cross = (points[i].y - points[p].y) * (points[q].x - points[i].x) -
                (points[i].x - points[p].x) * (points[q].y - points[i].y);
            
            if (cross < 0) q = i;
        }
        p = q;
    } while (p != leftmost);

    return hull;
}

std::vector<Point> GrahamAlgorithm::findConvexHull(std::vector<Point>& points)
{
    std::vector<Point> hull;

    if (points.size() < 3)
        return hull; // less than 3 points -> hull impossible

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

    // Filter points by renderable coordinates
    std::vector<Point> filtered;
    for (const auto& p : points) {
        if (p.x >= 0 && p.x <= 600 && p.y >= 0 && p.y <= 600)
            filtered.push_back(p);
    }
    points = filtered;

    // Find point with lowest y (and leftmost x if y equal)
    size_t lowest = 0;
    for (size_t i = 1; i < points.size(); ++i)
    {
        if (points[i].y < points[lowest].y ||
            (points[i].y == points[lowest].y && points[i].x < points[lowest].x))
        {
            lowest = i;
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

    // Remove collinear points (keep the farthest)
    std::vector<Point> uniquePoints;
    uniquePoints.push_back(points[0]);

    for (size_t i = 1; i < points.size(); ++i) 
    {
        // Skip points while they are collinear with previous
        while (i < points.size() - 1) {
            double cross = (points[i].y - start.y) * (points[i + 1].x - start.x) -
                (points[i].x - start.x) * (points[i + 1].y - start.y);
            if (std::abs(cross) < 1e-8) 
                i++; // collinear, skip
            else break;
        }
        uniquePoints.push_back(points[i]);
    }

    // Build convex hull using stack
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

            if (cross <= 0) // right turn or collinear
                stack.pop_back(); // remove point from stack
            else break; // left turn - good
        }
        stack.push_back(uniquePoints[i]);
    }

    return stack;
}