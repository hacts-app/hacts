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

const string path = "data.txt";

template < typename T >
const T linemax(vector<T> vector)
{
    sort(vector.begin(), vector.end());

    return vector[vector.size()-1];
}

void ai(Car* car, const double &delta)
{
    vector<double> radar = car->radar();
    double safety; // im dalej bandy tym wieksze safety i mocniej kreci kolami

    if(radar[0] < 2.33 || radar[1] < 4.5) // zbliza sie do krawedzi i awaryjnie odjeżdza
    {
        safety = min(radar[0]*1.93, radar[1]);
        car->humanChangeWheelAng(0.286*safety - 1.287); // ---
    }
    else if(radar[6] < 2.33 || radar[5] < 4.5)
    {
        safety = min(radar[6]*1.93, radar[5]);
        car->humanChangeWheelAng(-0.286*safety + 1.287); // +++
    }
    else if(radar[2] > radar[3] && radar[2] > radar[4]) // od tego miejsca szuka najlepszej drogi do jazdy
    {
        safety = min(radar[0]*1.93, radar[1]);
        car->humanChangeWheelAng(0.4*safety - 1.8); // +++
    }
    else if(radar[4] > radar[3] && radar[4] > radar[2])
    {
        safety = min(radar[6]*1.93, radar[5]);
        car->humanChangeWheelAng(-0.4*safety + 1.8);
    }
    else                                 // domyslnie trzyma kola rowno
        car->humanChangeWheelAng(0);

        // predkosc ...  wzor v = 0.4*s - 1 ... zawsze stara sie dazyc do tej predkosci
    if(car->getV() < 0.4*radar[3] - 1)
        car->onGasPush(0.3, delta);
    else
        car->onBrakePush(0.3, delta);

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

            car->humanChangeWheelAng(-values.second.second);

            if(values.second.second > 0)
                values.second.second -= 0.1 * delta;
            else if(values.second.second < 0)
                values.second.second += 0.1 * delta;

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
