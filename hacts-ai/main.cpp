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

map<int, Road> roads;

const string path = "data.txt";

template < typename T >
const T linemax(vector<T> vector)
{
    sort(vector.begin(), vector.end());

    return vector[vector.size()-1];
}

void ai(Car* &car, const double &delta, Road &road)
{
    vector<double> radar = car->radar(road.ways);

    if(radar[0] < 4) // zbliza sie do krawedzi i awaryjnie odjeżdza
        car->humanChangeWheelAng(0.1);
    else if(radar[6] < 4)
        car->humanChangeWheelAng(-0.1);
    else if(radar[3] == linemax(radar))  // od tego miejsca szuka najlepszej drogi do jazdy
        car->humanChangeWheelAng(0);
    else if(radar[2] == linemax(radar))
        car->humanChangeWheelAng(-0.1);
    else if(radar[4] == linemax(radar))
        car->humanChangeWheelAng(0.1);
    else if(radar[1] == linemax(radar))
        car->humanChangeWheelAng(-0.2);
    else if(radar[5] == linemax(radar))
        car->humanChangeWheelAng(0.2);
    else if(radar[0] == linemax(radar))
        car->humanChangeWheelAng(-0.5);
    else if(radar[6] == linemax(radar))
        car->humanChangeWheelAng(0.5);

    if(linemax(radar) > 90 && car->getV() < 20)
        car->onGasPush(0.5, delta);
    else if(linemax(radar) > 50 && car->getV() < 15)
        car->onGasPush(0.5, delta);
    else if(linemax(radar) > 15 && car->getV() < 10)
        car->onGasPush(0.3, delta);
    else if(linemax(radar) < 10 && car->getV() > 3)
        car->onBrakePush(1, delta);
}

void player(Car* &car, const double &delta)
{
    for(auto &values: switches)
    {
        if(car->getId() == values.first)
        {
            if(values.second.first == 1)
                car->onGasPush(1, delta);

            else if(values.second.first == -1)
                car->onBrakePush(1, delta);

            car->humanChangeWheelAng(values.second.second);
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

        for(auto &road: roads)
        {
            for(Car* &car: road.second.cars)
            {
                if(car->get_auto())
                    ai(car, delta, road.second);
                else
                    player(car, delta);

                car->changePos(delta);

                car->givePos();            
            }
            road.second.crashes();
        }

        delta = delta_t(start);

        if(delta < 0.03)
        {
            this_thread::sleep_for(chrono::milliseconds(int((0.03-delta)*1000)));
            delta = 0.03;
        }
    }

    return 0;
}
