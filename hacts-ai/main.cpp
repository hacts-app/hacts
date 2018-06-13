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


system_clock::time_point bef;

const int test_time =  12;
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

    bef = system_clock::now();

    Car golf3(0, 1540, 350, 3.23, 0.315, 54, 80, 0, 0, 4.02, 1.7);
    //Car tesla(1, 1540, 350, 3.23, 0.315, 54, 89, 30, 0, 4.02, 1.7); // z 1.9 TDI xddd

    roads[12].cars.push_back(&golf3);
    //roads[12].cars.push_back(&tesla);

    while(true) // klatka
    {
        this_thread::sleep_for(chrono::milliseconds(test_time));

        for(auto &road: roads)
        {
            for(Car* car: road.second.cars)
            {
                processCommand(inputHandler);

                car->radar(road.second.ways);

                car->onGasPush(1, bef);

                car->changePos(bef);

                car->givePos();

                road.second.crashes();
            }
        }
        bef = system_clock::now();
    }


    return 0;
}
