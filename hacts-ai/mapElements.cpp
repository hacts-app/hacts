#include "mapElements.h"

using namespace std;

void Road::crashes()
{
    vector<int> list_of_destroyed; // lista ktora przechowuje indexy umarlch w tej klatce aut

    for(unsigned int i = 0; i < cars.size(); i++) // wjazdy w sciany
    {
        if(!cars[i]->onRoad(leftWay) || !cars[i]->onRoad(rightWay))
        {
            list_of_destroyed.push_back(i);
        }
    }

    sort(list_of_destroyed.begin(), list_of_destroyed.end(), desc); // usuwanie od konca nie spowoduje
                                                                   // przesuniecia indexow w wektorze
    for(const int &x: list_of_destroyed) {
        broken_cars.push_back(new Rectangle(cars[x]->get_car_borders()->corners));
            // gdy auto umiera zostaje po nim prostokat jako przeszkoda dla innych aut

        cars.erase(cars.begin() + x, cars.begin() + x + 1);
    }

    list_of_destroyed.clear();

    for(unsigned int i = 0; i < cars.size(); i++) // zderzenia aut
    {
        for(unsigned int j = i + 1; j < cars.size(); j++)
        {
            if(cars[i]->get_car_borders()->intersection(cars[j]->get_car_borders())) // jesli nastapilo zderzenie
            {
                list_of_destroyed.push_back(i);

                list_of_destroyed.push_back(j);
            }
        }
        for(Rectangle* rec: broken_cars) // zderzenia z autami trupami
        {
            if(cars[i]->get_car_borders()->intersection(rec))
            {
                list_of_destroyed.push_back(i);

                break;
            }
        }
    }
    sort(list_of_destroyed.begin(), list_of_destroyed.end(), desc);

    for(const int &x: list_of_destroyed) {
        broken_cars.push_back(new Rectangle(cars[x]->get_car_borders()->corners));

        cars.erase(cars.begin() + x, cars.begin() + x + 1);
    }
}
/*
vector<Way> split_way(Way long_way)
{
    vector<Way> results;
    bool first = true;

    Node center = long_way.points[0];

    for(const Node &point: long_way.points)
    {
        if(first)
        {
            result.push_back(Way{point});
            first = false;
            continue;
        }
        if(point.x > center.x + 50 || point.x < center.x - 50 ||
           point.y > center.y + 50 || point.y < center.y - 50)
        {
            result.push_back(Way());
            center = point;
        }
        results.back().points.push_back(point);
    }
    return results;
}
*/







