#pragma once
#include "functions.h"
#include "car.h"

using namespace std;

class Car;
struct Rectangle;

struct Node
{
    double x;
    double y;
};

struct Way
{
    int id;
    vector<int> neighboursId = {};
    vector<Node> points = {};

    Way(int num, vector<int> neighbours)
    {
        id = num;
        neighboursId = neighbours;
    }
};

struct Road
{
    vector<Way> ways;
    vector<Car*> cars;
    vector<Rectangle*> broken_cars;

    void crashes(); // kraksa 2 samochodow oraz wjazdy w bande
};
