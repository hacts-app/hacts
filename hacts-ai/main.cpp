#include <iostream>
#include "lib.h"
#include <thread>
#include <chrono>
#include <iomanip>

using namespace std;

clock_t befPos;
clock_t befAct;

const int test_time =  25;

void pokaz(Car autko, int &i)
{
       cout << i * test_time / 1000.0 << " sekunda : ";

       autko.showPos();

       cout <<"  kat = " << autko.angle << " i V = " << autko.velocity<<endl;

       i++;
}

int main()
{
       befPos = clock();
       befAct = clock();

        Car golf3(1300, 350, 3.23, 0.315, 60, 0, 0, 0, 2);
    int i = 1;

    /*while(i*test_time <= 2000)
    {
           this_thread::sleep_for (chrono::milliseconds(test_time));

              golf3.changeWheelAng(1, befAct); // w sumie do 30 stopni
              golf3.changePos(befPos);

              pokaz(golf3, i);
    }*/

    while(i*test_time <= 10000)
       {
              this_thread::sleep_for (chrono::milliseconds(test_time));

              golf3.onGasPush(1, befAct);
              golf3.changePos(befPos);

              pokaz(golf3, i);
       }

    while(i*test_time <=12000)
       {
              this_thread::sleep_for (chrono::milliseconds(test_time));

              golf3.changePos(befPos);

              pokaz(golf3, i);
       }

       return 0;
}
