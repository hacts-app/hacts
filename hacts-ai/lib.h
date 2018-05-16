#include <iostream>

using namespace std;

#define PI 3.14159265

class Car
{
public:
    double velocity;
    double max_velocity;
    double acceleration;
    int mass;
    double max_transfer;
    int torque;
    double radius;
    double angle;
    int x,y;

public:
    Car(int m, double t, int tor, double r, double mv, double ang, int _x, int _y);
    // masa, ,max moment silnika, przełożenie, promien koła, max prędkość

    void onGasPush(double trans);
    // trans od 0.00 do 1 to % wciśnięcia gazu ... zakładamy że aktywowane co sekunde

       void onBrakePush(double per);
       // tak jak w gazie

       void ChangePos();
        // zmiana pozycji

};
