#include <iostream>
#include "lib.h"

using namespace std;

clock_t bef;

const int test_time =  12;
map<int, Road> roads;
const string path = "data.txt";

int main()
{
    //clog << "TESTTIME"<<endl;

    this_thread::sleep_for(chrono::milliseconds(4000));
    setRoads(path);

    vector<double> result;

    Car golf3(0, 1540, 350, 3.23, 0.315, 54, 0, 128, 520, 4.02, 1.7);

    bef = clock();

    golf3.changePos(bef);

    golf3.givePos();

    result = golf3.radar(roads[12].ways);

    for(double umiemTakXD : result)
    {
        clog << umiemTakXD << endl;
    }
/*
    while(golf3.getV() < 30) // faza przyspieszania
    {
        this_thread::sleep_for(chrono::milliseconds(test_time));

        golf3.onGasPush(1, bef);

        golf3.changePos(bef);

        golf3.givePos();

        bef = clock();
    }
/*
    while(golf3.getX() < 500) // do 500 metrow utrzymanie predkosci
    {
        this_thread::sleep_for(chrono::milliseconds(test_time));

        golf3.changePos(bef);

        golf3.givePos();

        bef = clock();
    }

    while(golf3.getV() > 2.7) // zwolnienie do 10 km/h
    {
        this_thread::sleep_for(chrono::milliseconds(test_time));

        golf3.onBrakePush(0.5, bef);

        golf3.changePos(bef);

        golf3.givePos();

        bef = clock();
    }


    while(golf3.getAng() < 45) // poczatek skretu
    {
        this_thread::sleep_for(chrono::milliseconds(test_time));

        golf3.changeWheelAng(0.5, bef);

        golf3.changePos(bef);

        golf3.givePos();

        bef = clock();
    }

    while(golf3.getWAng() > 0) // koniec skretu
    {
        this_thread::sleep_for(chrono::milliseconds(test_time));

        golf3.changeWheelAng(-0.5, bef);

        golf3.changePos(bef);

        golf3.givePos();

        bef = clock();
    }

    // Wojtkowe czity xd

    golf3.AngTo(90);
    golf3.WAngTo(0);


    while(golf3.getV() < 54) // gaz do dechy na max V
    {
        this_thread::sleep_for(chrono::milliseconds(test_time));

        golf3.onGasPush(1, bef);

        golf3.changePos(bef);

        golf3.givePos();

        bef = clock();
    }

    while(golf3.getV() > 0) // awaryjne hamowanie do 0
    {
        this_thread::sleep_for(chrono::milliseconds(test_time));

        golf3.onBrakePush(1, bef);

        golf3.changePos(bef);

        golf3.givePos();

        bef = clock();
    }
*/

    return 0;
}
