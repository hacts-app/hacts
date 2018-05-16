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
	Car(int m, double t, int tor, double r, double mv); // masa, ,max moment silnika, prze³o¿enie, promien ko³a, max prêdkoœæ

	void onPushGas(double trans); // trans od 0.00 do 1 to % wciœniêcia gazu ... zak³adamy ¿e aktywowane co sekunde

};
