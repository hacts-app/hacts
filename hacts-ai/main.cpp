#include <iostream>
#include "lib.h"

using namespace std;

int main()
{
    Car golf3(1300, 350, 3.23, 0.315, 60, 30, 0, 0);
    Car tesla(1220, 370, 8.28, 0.315, 111, 280, 0, 0);

    cout << "testy golf 3 vs tesla roadster"<<endl<<endl;

    int i = 0;

       for(; i <25; i++)
       {
              golf3.onGasPush(0.5);
              tesla.onGasPush(0.2);

              golf3.ChangePos();
              tesla.ChangePos();

              cout << i +1<<" sekunda\n\t golf: "<<golf3.velocity<<" m/s ";
              cout<<",tesla: "<<tesla.velocity<<" m/s."<<endl;

              cout << "\t Pozycja golfa: ("<<golf3.x << ", " << golf3.y << ")"<<endl;
              cout << "\t Pozycja tesli: ("<<tesla.x << ", " << tesla.y << ")"<<endl;
       }
       i = 0;

       cout << endl  << "\t\tHAMOWANIE" << endl << endl;

       while(golf3.velocity > 0 || tesla.velocity > 0)
       {
              golf3.onBrakePush(1);
              tesla.onBrakePush(1);

              golf3.ChangePos();
              tesla.ChangePos();

              cout << i +1<<" sekunda\n\t golf: "<<golf3.velocity<<" m/s ";
              cout<<",tesla: "<<tesla.velocity<<" m/s."<<endl;

              cout << "\t Pozycja golfa: ("<<golf3.x << ", " << golf3.y << ")"<<endl;
              cout << "\t Pozycja tesli: ("<<tesla.x << ", " << tesla.y << ")"<<endl;

              i++;
       }


    return 0;
}
