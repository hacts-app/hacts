#include <iostream>
#include "lib.h"
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


double delta; // w sek.

const int max_fps =  60;
map<int, Road> roads;
const int max_road_nr = 12;
const string path = "data.txt";

static void processCommand(const std::string &command) {
    std::clog << "Received " << command;
}

static void processCommands(InputHandler &inputHandler) {
    std::string command;
    while(inputHandler.getAvailableInput(command)) {
        if(command == "pause") {
            for(;;) {
                inputHandler.waitForInput(command);
                if(command == "resume")
                    return;

                processCommand(command);
            }
        } else {
            processCommand(command);
        }
    }
}

int main()
{

    fixPlatformQuirks();

    InputHandler inputHandler;

    setRoads(path);

    vector<double> radar;

    system_clock::time_point start;

    delta = 0;

    Car golf3(0, 1540, 350, 3.23, 0.315, 54, 80, 0, 0, 4.02, 1.7);
    //Car tesla(1, 1540, 350, 3.23, 0.315, 54, 89, 30, 0, 4.02, 1.7); // z 1.9 TDI xddd

    roads[12].cars.push_back(&golf3);
    //roads[12].cars.push_back(&tesla);

    while(true) // klatka
    {
        start = system_clock::now();

        for(auto &road: roads)
        {
            for(Car* car: road.second.cars)
            {
                processCommands(inputHandler);

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
