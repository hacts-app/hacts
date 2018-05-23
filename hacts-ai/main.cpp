#include <iostream>
#include "lib.h"

using namespace std;

clock_t befPos;
clock_t befAct;

const int test_time =  50;
map<int, Road> roads;
const string path = "data.txt";

int main()
{

    setRoads(path);
    system("cd");

    befPos = clock();
    befAct = clock();

    Car golf3(0, 1540, 350, 3.23, 0.315, 54, 0, 0, 0, 4.02, 1.7);

    golf3.showPos();
    golf3.givePos();

    clog << roads[12].ways.size() << endl;

    return 0;
}
