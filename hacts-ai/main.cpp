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
system_clock::time_point start;

vector<int> cars_id {0};

map<int, Road> roads;

const int max_road_nr = 12;

const string path = "data.txt";

static void processCommand(const std::string &command) {
    if(command == "")
        return;

    vector<string> parameters = split(command, ' ');

    if(parameters[0] == "newcar" && parameters.size() == 2)
    {
        int id = stoi(parameters[1]);

        for(const int &x: cars_id)
        {
            if(id == x)
                return;
        }
        roads[12].cars.push_back(new Car(id, 1540, 350, 3.23, 0.315, 54, 0, 0, 0, 4.02, 1.7));

        cars_id.push_back(id);

        clog << "Car " << id << " added" << endl;

        return;
    }

    clog << "Received wrong command! \"" << command << "\"" << endl;
}

static void processCommands(InputHandler &inputHandler) {
    std::string command;
    while(inputHandler.getAvailableInput(command)) {
        if(command == "pause") {
            for(;;) {
                inputHandler.waitForInput(command);
                if(command == "resume")
                {
                    start = system_clock::now();
                    return;
                }
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



    delta = 0;

    Car golf3(0, 1540, 350, 3.23, 0.315, 54, 80, 0, 0, 4.02, 1.7);
    //Car tesla(1, 1540, 350, 3.23, 0.315, 54, 89, 30, 0, 4.02, 1.7); // z 1.9 TDI xddd

    roads[12].cars.push_back(&golf3);
    //roads[12].cars.push_back(&tesla);

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
