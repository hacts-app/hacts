#include <iostream>

using namespace std;

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

public:
	Car(int m, double t, int tor, double r, double mv); // masa, ,max moment silnika, prze�o�enie, promien ko�a, max pr�dko��

	void onPushGas(double trans); // trans od 0.00 do 1 to % wci�ni�cia gazu ... zak�adamy �e aktywowane co sekunde

};
