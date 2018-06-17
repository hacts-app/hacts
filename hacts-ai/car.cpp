#include "car.h"
#include "rectangle.h"

using namespace std;

Car::Car(int id, int mass, double transfer, int torque, double radius, double max_velocity,
         double angle, double _x, double _y, double length, double width)
{
    carId = id;
    this->mass = mass;
    max_transfer = transfer;
    this->torque = torque;
    this->radius = radius;
    this->max_velocity = max_velocity;
    this->angle = angle;
    x = _x;
    y = _y;
    velocity = 0;
    this->length = length;
    this->width = width;
    wheelAng = 0;

    double R = (sqrt(pow(length, 2) + pow(width, 2))) / 2;
    // promien okregu opisanego na samochodzie

    double alpha = acos(1-(pow(width, 2)) / (2 * pow(R, 2))) * 180.0 / PI;
    // kat miedzy dwoma R-ami naprzeciwko szerokosci pojazdu

    double a1 = angle + alpha/2;
    // kat prostej 1R

    double a2 = angle - alpha/2;
    // kat prostej 2R

    car_borders = new Rectangle({Node{0,0}});

    vector<Node> carCorners{
        moveNode(x, y, a1, R),
        moveNode(x, y, a2, R),
        moveNode(x, y, a1, -R),
        moveNode(x, y, a2, -R),
    }; // ABCD ...

    car_borders->update(carCorners);
}

void Car::onGasPush(double trans, double delta) // trans od 0.00 do 1 to % wciœniêcia gazu ... zak³adamy ¿e aktywowane co sekunde
{
    double transfer = max_transfer * trans;

    double F = (transfer * torque) / radius;

    acceleration = F / mass;

    velocity += acceleration * delta;

    if(velocity > max_velocity)
        velocity = max_velocity;
}

void Car::onBrakePush(double per, double delta)
{
    const double max_acc = -9.59; // to chyba jest max opoznienie w hamowaniu

    acceleration = max_acc * per;

    velocity += acceleration * delta;

    if(velocity < 0)            // hamulec nie jest biegiem wstecznym
        velocity = 0;
}

void Car::changeWheelAng(double intensity, double delta)
{
    const int max_rot = 15 ; // maksymalna predkosc zmiany kata  kol w sekundzie

    double rot = max_rot * intensity;

    wheelAng += rot * delta;

    if(wheelAng > 40)
        wheelAng = 40;
    else if(wheelAng < -40)
        wheelAng = -40;
}

void Car::humanChangeWheelAng(double position)
{
    position *= 40;

    wheelAng = position;
}

vector<double> Car::radar(vector<Way> &ways)
{
    vector<double> result;
    double minimum = 100;

    for(int k = 0; k < 7; k++)
    {
        double ang;

        if(k == 0 )
            ang = angle + 30;
        else if(k == 1)
            ang = angle + 15;
        else if(k == 2)
            ang = angle + 5;
        else if(k == 3)
            ang = angle;
        else if(k == 4)
            ang = angle - 5;
        else if(k == 5)
            ang = angle - 15;
        else
            ang = angle - 30;

        for(const Way &way : ways)
        {
            for(unsigned int j = 1; j < way.points.size(); j++)
            {
                double xA = way.points[j-1].x;
                double yA = way.points[j-1].y;

                double xB = way.points[j].x;
                double yB = way.points[j].y;

                double tmp = dist(ang, x, y, xA, yA, xB, yB);

                if(tmp < minimum)
                    minimum = tmp;
            }
        }
        result.push_back(minimum);

        minimum = 100;
    }
    return result;
}

bool Car::onRoad(vector<Way> &ways)
{
    double R = (sqrt(pow(length, 2) + pow(width, 2))) / 2;
    // promien okregu opisanego na samochodzie

    double alpha = acos(1-(pow(width, 2)) / (2 * pow(R, 2))) * 180.0 / PI;
    // kat miedzy dwoma R-ami naprzeciwko szerokosci pojazdu

    double a1 = angle + alpha/2;
    // kierunkowa 1 R

    double a2 = angle - alpha/2;
    // kierunkowa 2 R

    vector<Node> carCorners{
        moveNode(x, y, a1, R),
        moveNode(x, y, a2, R),
        moveNode(x, y, a1, -R),
        moveNode(x, y, a2, -R),
    }; // ABCD ...

    car_borders->update(carCorners);

    for(const Way &way: ways)
    {
        for(unsigned int i = 1; i < way.points.size(); i++)
        {
            Node A = way.points[i-1];
            Node B = way.points[i];

            if(car_borders->intersection(A, B)) // jesli przeciecie zwraca prawde
                return false;                   // to auto nie jest na drodze

        }
    }
    return true;
}

void Car::showPos()
{
    clog << "(x = " << x << ", y = " << y <<") Angle = " << angle << " V = " << velocity <<endl;
}

void Car::givePos()
{
    cout <<"movecar "<< carId <<" "<< x <<" "<< y <<" "<< angle <<endl;
}

void Car::changePos(double delta)
{
    angle += radToDeg( (2 * velocity * sin(wheelAng * PI / 180.0)) / (length-0.6) ) * delta;

    if(angle > 360)
        angle -= 360;

    if(angle < 0)
        angle += 360;

    x += velocity * cos((angle) * PI / 180.0) * delta;

    y += velocity * sin((angle) * PI / 180.0) * delta;
}


