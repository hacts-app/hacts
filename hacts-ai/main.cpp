#include <iostream>
#include "lib.h"

using namespace std;

int main() {
    Car golf3(1300, 350, 3.23, 0.315, 60);
    Car tesla(1220, 370, 8.28, 0.315, 111);

    cout << "testy golf 3 vs tesla roadster"<<endl<<endl;

    for(int i = 0; i <25; i++)
    {
        golf3.onPushGas(1);
        tesla.onPushGas(1);

        cout << i +1<<" sekunda golf: "<<golf3.velocity<<" m/s ";
        cout<<",tesla: "<<tesla.velocity<<" m/s."<<endl;
    }

}
