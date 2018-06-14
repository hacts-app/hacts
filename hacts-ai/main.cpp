#include <iostream>
#include "lib.h"
#include "inputhandler.h"
#include "processCommand.h"

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


double delta = 0; // w sek.
system_clock::time_point start;

vector<int> cars_id {};

map<int, Road> roads;

const int max_road_nr = 12;

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

                car->onGasPush(1, delta);

                car->changePos(delta);

                road.second.crashes();

                car->givePos();            
            }
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
