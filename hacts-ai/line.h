#pragma once

struct Point {
    double x;
    double y;
};

class Line
{
public:
    Line(Point pt1, Point pt2) : pt1(pt1), pt2(pt2) {}

    Point pt1;
    Point pt2;

    bool intersects(const Line &l, Point *intersectionPoint) const;

    /*
     * Returns the angle of a line in radians.
     *
     *  Positive values for the angles mean counter-clockwise while negative values
     *  mean the clockwise direction. Zero degrees is at the 3 o'clock position.
     */
    double angle() const;
};
