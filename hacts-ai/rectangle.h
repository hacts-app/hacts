#pragma once
#include "functions.h"
#include "car.h"
#include "mapElements.h"

using namespace std;

struct Node;

struct Rectangle
{
    vector<Node> corners;

    Rectangle(vector<Node> points)
    {
        corners = points;
    }

    void update(vector<Node> points)
    {
        corners = points;
    }

    bool intersection(Node A, Node B); // true jesli odcinek AB przecina prostokat

    bool intersection(Rectangle *car); // true jesli prostokaty maja punkt stycznosci
};
