#include <iostream>
#include "lib.h"
//#include "inputhandler.h"



using namespace std;
using namespace chrono;
//
system_clock::time_point bef;

const int test_time =  12;
map<int, Road> roads;
const string path = "data.txt";

/*
static void processCommand(InputHandler &inputHandler) {
    std::string command;
    while(inputHandler.getAvailableInput(command))
        std::clog << "Received " << command;
}
*/
int main()
{
   // InputHandler inputHandler;

    bef = system_clock::now();
    this_thread::sleep_for(milliseconds(4000));

    setRoads(path);

    vector<double> radar;

    Car golf3(0, 1540, 350, 3.23, 0.315, 54, 0, 0, 0, 4.02, 1.7); // xy 128, 520

    bef = system_clock::now();

    golf3.changePos(bef);

    golf3.givePos();


    while(golf3.getV() < 30) // faza przyspieszania
    {
    //    processCommand(inputHandler);
        this_thread::sleep_for(chrono::milliseconds(test_time));

        golf3.onGasPush(1, bef);

        golf3.changePos(bef);

        golf3.givePos();

        radar = golf3.radar(roads[12].ways);
//        return 0;

        if(!golf3.onRoad(roads[12].ways))
            return 0;

        bef = system_clock::now();
    }

    while(golf3.getX() < 500) // do 500 metrow utrzymanie predkosci
    {
     //   processCommand(inputHandler);
        this_thread::sleep_for(chrono::milliseconds(test_time));

        golf3.changePos(bef);

        golf3.givePos();

        radar = golf3.radar(roads[12].ways);

        if(!golf3.onRoad(roads[12].ways))
            return 0;

        bef = system_clock::now();
    }

    while(golf3.getV() > 2.7) // zwolnienie do 10 km/h
    {
      //  processCommand(inputHandler);
        this_thread::sleep_for(chrono::milliseconds(test_time));

        golf3.onBrakePush(0.5, bef);

        golf3.changePos(bef);

        golf3.givePos();

        radar = golf3.radar(roads[12].ways);

        if(!golf3.onRoad(roads[12].ways))
            return 0;

        bef = system_clock::now();
    }


    while(golf3.getAng() < 45) // poczatek skretu
    {
      //  processCommand(inputHandler);
        this_thread::sleep_for(chrono::milliseconds(test_time));

        golf3.changeWheelAng(0.5, bef);

        golf3.changePos(bef);

        golf3.givePos();

        radar = golf3.radar(roads[12].ways);

        if(!golf3.onRoad(roads[12].ways))
            return 0;

        bef = system_clock::now();
    }

    while(golf3.getWAng() > 0) // koniec skretu
    {
       // processCommand(inputHandler);
        this_thread::sleep_for(chrono::milliseconds(test_time));

        golf3.changeWheelAng(-0.5, bef);

        golf3.changePos(bef);

        golf3.givePos();

        radar = golf3.radar(roads[12].ways);

        if(!golf3.onRoad(roads[12].ways))
            return 0;

        bef = system_clock::now();
    }

    golf3.AngTo(90);
    golf3.WAngTo(0);


    while(golf3.getV() < 54) // gaz do dechy na max V
    {
      //  processCommand(inputHandler);
        this_thread::sleep_for(chrono::milliseconds(test_time));

        golf3.onGasPush(1, bef);

        golf3.changePos(bef);

        golf3.givePos();

        radar = golf3.radar(roads[12].ways);

        if(!golf3.onRoad(roads[12].ways))
            return 0;

        bef = system_clock::now();
    }

    while(golf3.getV() > 0) // awaryjne hamowanie do 0
    {
       // processCommand(inputHandler);
        this_thread::sleep_for(chrono::milliseconds(test_time));

        golf3.onBrakePush(1, bef);

        golf3.changePos(bef);

        golf3.givePos();

        radar = golf3.radar(roads[12].ways);

        if(!golf3.onRoad(roads[12].ways))
            return 0;

        bef = system_clock::now();
    }


    return 0;
}
