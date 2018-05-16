#include <iostream>
#include "lib.h"

using namespace std;

Car::Car(int m, double t, int tor, double r, double mv) // masa, ,max moment silnika, prze�o�enie, promien ko�a, max pr�dko��
	{
              mass = m;

              max_transfer = t;

              torque = tor;

              radius = r;

              max_velocity = mv;

              velocity = 0;
	}

void Car::onPushGas(double trans) // trans od 0.00 do 1 to % wci�ni�cia gazu ... zak�adamy �e aktywowane co sekunde
	{
		double transfer = max_transfer * trans;

		double F = (transfer * torque) / radius;

		acceleration = F / mass;

		velocity += acceleration * 1; // zakladajac �e co sekunde

		if(velocity > max_velocity)
			velocity = max_velocity;
	}
