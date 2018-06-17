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

void ai(Car* &car, const double &delta, Road &road)
{
    car->radar(road.ways);

    car->changeWheelAng(1, delta);

    car->onGasPush(1, delta);
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
