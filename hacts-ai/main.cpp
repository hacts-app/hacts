#include <iostream>
#include "functions.h"
#include "inputhandler.h"

using namespace std;
using namespace chrono;

#ifdef _WIN32
#include <windows.h>
static void fixPlatformQuirks() {
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    }
}
#else
static void fixPlatformQuirks() {
}
#endif

map<int, pair<int, double>> switches;

double delta = 0; // w sek.

system_clock::time_point start;

vector<int> cars_id {};

vector<Car*> cars;
vector<Road*> all_roads;

string path = "data.json";

double vector_max(vector<double> vec)
{
    sort(vec.begin(), vec.end());
    return vec[vec.size()-1];
}

void ai(Car* car, const double &delta)
{
    vector<double> radar = car->radar();
    double safety; // im dalej bandy tym wieksze safety i mocniej kreci kolami

    for(double &element: radar) {
        element = fabs(element);
    }

    if(radar[6] < 0.375 || radar[5] < 1.125)
        {
            safety = min(radar[6]*3, radar[5]);
            car->quickChangeWheelAng(-safety + 1.125); // <---
        }
    else if(radar[0] < 0.375 || radar[1] < 1.125) // zbliza sie do krawedzi i awaryjnie odjeżdza
    {
        safety = min(radar[0]*3, radar[1]);
        car->quickChangeWheelAng(safety - 1.125); // --->    ang = a * safety + b
    }
    else if(radar[2] > radar[3] && radar[2] > radar[4] && car->getWheelAng() < 5) // od tego miejsca szuka najlepszej drogi do jazdy
    {
        safety = min(radar[0]*3, radar[1]);
        car->smoothChangeWheelAng(0.5*safety - 0.5, delta); // <---
    }
    else if(radar[4] > radar[3] && radar[4] > radar[2] && car->getWheelAng() > -5)
    {
        safety = min(radar[6]*3, radar[5]);
        car->smoothChangeWheelAng(-0.5*safety + 0.5, delta); // --->
    }
    else                                 // domyslnie dazy do rownych kol
        car->smoothChangeWheelAng(0, delta);

        // predkosc ...  wzor v = 0.5*s - 1 ... zawsze stara sie dazyc do tej predkosci
    double maximum = vector_max({radar[2], radar[3], radar[4]});

    if(car->getV() < 0.5*maximum - 1)
        car->onGasPush(0.8, delta);
    else
        car->onBrakePush(1, delta);

}

void player(Car* car, const double &delta)
{
    for(auto &values: switches)
    {
        if(car->getId() == values.first)
        {
            if(values.second.first == 1)
                car->onGasPush(1, delta);

            else if(values.second.first == -1)
                car->onBrakePush(1, delta);

            car->quickChangeWheelAng(-values.second.second);

            break;
        }
    }
}

int main()
{

    fixPlatformQuirks();

    InputHandler inputHandler;

    setRoads(path);

    vector<double> radar;

    while(true) // klatka
    {
        start = system_clock::now();

        processCommands(inputHandler);

        for(Car* car: cars)
        {
            if(car->get_auto())
                ai(car, delta);
            else
                player(car, delta);

            car->changePos(delta);

            car->givePos();
        }
        for(Road* road: all_roads)
            road->crashes();

        delta = delta_t(start);

        if(delta < 0.03)
        {
            this_thread::sleep_for(chrono::milliseconds(int((0.03-delta)*1000)));
            delta = 0.03;
        }
    }

    return 0;
}
