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
    long long id;
    vector<Way> ways;
    vector<Rectangle*> broken_cars;

    Road(long long nr)
    {
        id = nr;
    }

    void crashes(); // kraksa 2 samochodow oraz wjazdy w bande
};
