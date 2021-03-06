#pragma once
#include "functions.h"
#include "car.h"

using namespace std;

class Car;
struct Rectangle;

extern vector<Car*> cars;

struct Node
{
    double x;
    double y;

    bool compare(Node &other)
    {
        if(this->x == other.x && this->y == other.y)
            return true;
        return false;
    }
};

struct Way
{
    vector<Node> points;
};

struct Lane
{
    Way left_border;
    Way right_border;
};

struct Road
{
    long long id;
    Way leftWay;
    Way rightWay;
    vector<Lane> lanes;
    vector<Rectangle*> broken_cars;

    Road(long long nr)
    {
        id = nr;
    }

    void crashes(); // kraksa 2 samochodow oraz wjazdy w bande
};

