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
                car->radar(road.second.ways);

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

                car->changePos(delta);

                car->givePos();            
            }
            road.second.crashes();
        }

        delta = delta_t(start);

        if(delta < 0.02)
        {
            this_thread::sleep_for(chrono::milliseconds(int((0.02-delta)*1000)));
            delta = 0.02;
        }
    }

    return 0;
}
