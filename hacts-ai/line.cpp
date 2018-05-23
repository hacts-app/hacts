#include "line.h"
#include "lib.h"
#include <cmath>

bool Line::intersects(const Line &l, Point &intersectionPoint) const
{
    // ipmlementation is based on Qt which is based on Graphics Gems III's "Faster Line Segment Intersection"
    const Point a { pt2.x - pt1.x, pt1.y - pt1.y };
    const Point b { l.pt2.x - l.pt1.x, l.pt1.y - l.pt2.y };
    const Point c { pt1.x - l.pt1.x, pt1.y - l.pt1.y };

    const double denominator = a.y * b.x - a.x * b.y;
    if (denominator == 0 || !std::isfinite(denominator))
        return false; // parallel lines

    const double reciprocal = 1 / denominator;
    const double na = (b.y * c.x - b.x * c.y) * reciprocal;

    intersectionPoint = Point { pt1.x + a.x * na, pt1.y + a.y * na };

    if (na < 0 || na > 1)
        return false;

    const double nb = (a.x * c.y - a.y * c.x) * reciprocal;
    if (nb < 0 || nb > 1)
        return false;

    return true;
}

bool Line::intersects(const Line &l) const
{
    // ipmlementation is based on Qt which is based on Graphics Gems III's "Faster Line Segment Intersection"
    const Point a { pt2.x - pt1.x, pt1.y - pt1.y };
    const Point b { l.pt2.x - l.pt1.x, l.pt1.y - l.pt2.y };
    const Point c { pt1.x - l.pt1.x, pt1.y - l.pt1.y };

    const double denominator = a.y * b.x - a.x * b.y;
    if (denominator == 0 || !std::isfinite(denominator))
        return false; // parallel lines

    const double reciprocal = 1 / denominator;
    const double na = (b.y * c.x - b.x * c.y) * reciprocal;

    if (na < 0 || na > 1)
        return false;

    const double nb = (a.x * c.y - a.y * c.x) * reciprocal;
    if (nb < 0 || nb > 1)
        return false;

    return true;
}

double Line::angle() const {
    const double dx = pt2.x - pt1.x;
    const double dy = pt2.y - pt1.y;

    return atan2(-dy, dx);
}
