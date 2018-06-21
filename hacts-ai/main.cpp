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

void ai(Car* car, const double &delta)
{
    vector<double> radar = car->radar();
    double safety; // im dalej bandy tym wieksze safety i mocniej kreci kolami

    if(radar[6] < 1.25 || radar[5] < 2.412)
        {
            safety = min(radar[6]*1.93, radar[5]);
            car->quickChangeWheelAng(-safety + 2.412); // <---
        }
    else if(radar[0] < 1.25 || radar[1] < 2.412) // zbliza sie do krawedzi i awaryjnie odjeżdza
    {
        safety = min(radar[0]*1.93, radar[1]);
        car->quickChangeWheelAng(safety - 2.412); // --->    ang = a * safety + b
    }
    else if(radar[2] > radar[3] && radar[2] > radar[4] && car->getWheelAng() < 6) // od tego miejsca szuka najlepszej drogi do jazdy
    {
        safety = min(radar[0]*1.93, radar[1]);
        car->smoothChangeWheelAng(0.2*safety - 0.9, delta); // <---
    }
    else if(radar[4] > radar[3] && radar[4] > radar[2] && car->getWheelAng() > -6)
    {
        safety = min(radar[6]*1.93, radar[5]);
        car->smoothChangeWheelAng(-0.2*safety + 0.9, delta); // --->
    }
    else                                 // domyslnie dazy do rownych kol
        car->smoothChangeWheelAng(0, delta);

        // predkosc ...  wzor v = 0.5*s - 1 ... zawsze stara sie dazyc do tej predkosci
    if(car->getV() < 0.5*(radar[3]-0.5*car->getLen()) - 1)
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
/*
            if(values.second.second > 0)
                values.second.second -= 0.4 * delta;
            else if(values.second.second < 0)
                values.second.second += 0.4 * delta;
*/
            break;
        }
    }
}



// example:

/*

void readJson() {
    ifstream ifs("../hacts-ai/test.json");
    json::IStreamWrapper stream(ifs);

    json::Document doc;
    doc.ParseStream(stream);

    clog << doc["to"]["jest"]["przyklad"].GetString() << "\n";

    for(const auto &element : doc["to"]["tablica"].GetArray()) {
        clog << element["costam"].GetInt64() << "\n";
    }

    for(const auto &element : doc["to"]["jest"]["jakis objekt"].GetObject()) {
        clog << element.name.GetString() << " = " << element.value.GetString() << "\n";
    }
}

*/



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
