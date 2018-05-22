#include <iostream>
#include "lib.h"

using namespace std;

clock_t befPos;
clock_t befAct;

const int test_time =  50;
map<int, Road> roads;
const string path = "C:\\Users\\Wojciech\\Desktop\\potrzebne\\programy stworzone\\projekt\\hacts\\build-hacts-ai-Desktop_Qt_5_10_1_MinGW_32bit2-Debug\\debug\\data.txt";

int main()
{

    setRoads(path);

    befPos = clock();
    befAct = clock();

    Car golf3(1540, 350, 3.23, 0.315, 54, 0, 0, 0, 4.02, 1.7);

    golf3.showPos();

    cout << roads[12].ways.size() << endl;

    return 0;
}
