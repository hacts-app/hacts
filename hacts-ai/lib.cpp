#include <iostream>
#include "lib.h"
#include <cmath>

using namespace std;

Car::Car(int m, double t, int tor, double r, double mv, double ang, int _x, int _y)
              // masa, ,max moment silnika, przełożenie, promien koła, max prędkość
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
    }

void Car::onGasPush(double trans) // trans od 0.00 do 1 to % wciśnięcia gazu ... zakładamy że aktywowane co sekunde
    {
        double transfer = max_transfer * trans;

        double F = (transfer * torque) / radius;

        acceleration = F / mass;

        velocity += acceleration * 1; // zakladajac że co sekunde

        if(velocity > max_velocity)
            velocity = max_velocity;
    }

void Car::onBrakePush(double per)
       {
           const double max_acc = -9.59; // to chyba jest max opoznienie w hamowaniu

           acceleration = max_acc * per;

           velocity += acceleration * 1;  // zakladajac że co sekunde

           if(velocity < 0)            // hamulec nie jest biegiem wstecznym
               velocity = 0;
       }

void Car::ChangePos()
       {
           x += round(velocity * cos(angle * PI / 180.0));

           y += round(velocity * sin(angle * PI / 180.0));
       }
