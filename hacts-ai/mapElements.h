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
};

struct Way
{
    vector<Node> points;
};

struct Lane
{
    Way loWay;
    Way hiWay;
};

struct Road
{
    long long id;
    Way loWay;
    Way hiWay;
    vector<Lane> lanes;
    vector<Rectangle*> broken_cars;

    Road(long long nr)
    {
        id = nr;
    }

    void crashes(); // kraksa 2 samochodow oraz wjazdy w bande
};

