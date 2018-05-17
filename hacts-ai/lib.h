#include <iostream>
#include <ctime>
#include <vector>
using namespace std;

#define PI 3.14159265

clock_t delta_t(clock_t &bef);

double radToDeg(double rad);

double degToRad(double deg);

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
    double wheelAng;
    double x,y;
    double length;

public:
    Car(int m, double t, int tor, double r, double mv, double ang, double _x, double _y, double len);
    // masa, ,max moment silnika, prze³o¿enie, promien ko³a, max prêdkoœæ

    void onGasPush(double trans, clock_t &bef);
    // trans od 0.00 do 1 to % wciœniêcia gazu ... zak³adamy ¿e aktywowane co sekunde

       void onBrakePush(double per, clock_t &bef);
       // tak jak w gazie

       void changeWheelAng(double intensity, clock_t &bef);
       // od -1 do 1 .. ujemne skrecają w prawo dodatnie w lewo

       void showPos();

       void changePos(clock_t &bef);
        // zmiana pozycji i KĄTA

};
