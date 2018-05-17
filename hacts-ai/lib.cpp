#include <iostream>
#include "lib.h"
#include <cmath>

using namespace std;



clock_t delta_t(clock_t &bef)
{
       clock_t tmp = bef;

       bef = clock();

       return bef - tmp;
}

double radToDeg(double rad)
{
       return (180.0 * rad) / PI;
}

double degToRad(double deg)
{
       return (PI * deg) / 180.0;
}

Car::Car(int m, double t, int tor, double r, double mv, double ang, double _x, double _y, double len)
              // masa, ,max moment silnika, przelozenie, promien kola, max predkosc
    {
              mass = m;

              max_transfer = t;

              torque = tor;

              radius = r;

              max_velocity = mv;

              angle = ang;

              x= _x;

              y = _y;

              velocity = 0;

              length = len;

              wheelAng = 0;
    }

void Car::onGasPush(double trans, clock_t &bef) // trans od 0.00 do 1 to % wciœniêcia gazu ... zak³adamy ¿e aktywowane co sekunde
    {
        double transfer = max_transfer * trans;

        double F = (transfer * torque) / radius;

        acceleration = F / mass;

        velocity += (acceleration * pow(delta_t(bef), 1) * pow(0.001, 1)); // kom

        if(velocity > max_velocity)
            velocity = max_velocity;
    }

void Car::onBrakePush(double per, clock_t &bef)
       {
              const double max_acc = -9.59; // to chyba jest max opoznienie w hamowaniu

              acceleration = max_acc * per;

              velocity += (acceleration * pow(delta_t(bef), 1) * pow(0.001, 1));  //kom

              if(velocity < 0)            // hamulec nie jest biegiem wstecznym
                     velocity = 0;
       }

void Car::changeWheelAng(double intensity, clock_t &bef)
       {
              const int max_rot = 15 ; // maksymalna predkosc zmiany kata  kol w sekundzie

              double rot = max_rot * intensity;

              wheelAng += (rot * pow(delta_t(bef), 1) * pow(0.001, 1));

              if(wheelAng > 40)
                     wheelAng = 40;
              else if(wheelAng < -40)
                     wheelAng = -40;
       }

void Car::showPos()
       {
              cout << "(x = " << x << ", y = " << y <<")";
       }

void Car::changePos(clock_t &bef)
       {
              vector<clock_t> tmp(3, bef);

              angle += radToDeg( (2 * velocity * sin(wheelAng * PI / 180.0)) / length ) * pow(delta_t(tmp[0]), 1) * pow(0.001, 1);

              if(angle > 360)
                     angle -= 360;

              if(angle < 0)
                     angle += 360;

              x += (velocity * cos(angle * PI / 180.0) * 10 * pow(delta_t(tmp[1]), 1) * pow(0.001, 1));

              y += (velocity * sin(angle * PI / 180.0) * 10 * pow(delta_t(tmp[2]), 1) * pow(0.001, 1));

              bef = clock();
       }


