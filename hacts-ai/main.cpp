#include <iostream>
#include "lib.h"


using namespace std;

clock_t bef;

int main()
{
       bef = clock();
        Car golf3(1300, 350, 3.23, 0.315, 60, 0, 0, 0, 2);
    int i = 200;

    golf3.changeWheelAng(1);
    golf3.changeWheelAng(1); // w sumie do 30 stopni

    golf3.onGasPush(0.5);
    golf3.onGasPush(0.5);
    golf3.onGasPush(0.5);

    while(i--)
       {


              golf3.changePos();

              cout << 200 - i << " sekunda : ";

              golf3.showPos();

              cout <<"  kat = " << golf3.angle<<endl;
       }


       return 0;
}
