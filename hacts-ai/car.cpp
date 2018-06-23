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
    auto_drive = true;

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

void Car::smoothChangeWheelAng(double position, double delta)
{
    const int max_rot = 15;

    if(position > 1)
        position = 1;
    else if(position < -1)
        position = -1;

    position *= 40;

    if(fabs(position - wheelAng) > max_rot * delta) // jesli roznica oczekiwanego kata i prawdziwego jest
    {                                       // wieksza niz 15 stopni na sekunde
        if(position > wheelAng)
            wheelAng += max_rot * delta;
        else
            wheelAng -= max_rot * delta;
    }
    else
        wheelAng = position;

    if(wheelAng > 40)
        wheelAng = 40;
    if(wheelAng < -40)
        wheelAng = -40;

}

void Car::quickChangeWheelAng(double position)
{
    if(position > 1)
        position = 1;
    else if(position < -1)
        position = -1;

    position *= 40;

    wheelAng = position;
}

vector<double> Car::radar()
{
    vector<double> result; 
    double tmp;

    for(int k = 0; k < 7; k++)
    {
        double ang;
        double minimum = 100;

        if(k == 0 )
            ang = angle + 90;
        else if(k == 1)
            ang = angle + 20;
        else if(k == 2)
            ang = angle + 5;
        else if(k == 3)
            ang = angle;
        else if(k == 4)
            ang = angle - 5;
        else if(k == 5)
            ang = angle - 20;
        else
            ang = angle - 90;
        for(Road* road: roads) // dla kazdej drogi na jakiej miesci sie samochod
        {
            for(Lane &lane: road->lanes) // zbadaj kazdy pas na tej drodze
            {
                for(unsigned int j = 1; j < lane.left_border.points.size(); j++)
                {
                    tmp = dist(ang, x, y,
                               lane.left_border.points[j-1].x, lane.left_border.points[j-1].y,
                               lane.left_border.points[j].x, lane.left_border.points[j].y);

                    if(tmp < minimum)
                        minimum = tmp;
                }
                for(unsigned int j = 1; j < lane.right_border.points.size(); j++)
                {
                    tmp = dist(ang, x, y,
                               lane.right_border.points[j-1].x, lane.right_border.points[j-1].y,
                               lane.right_border.points[j].x, lane.right_border.points[j].y);

                    if(tmp < minimum)
                        minimum = tmp;
                }

            }

            for(Rectangle* dead_car: road->broken_cars)
            {
                for(int i = 0; i < 4; i++)
                {
                    if(i == 0)
                    {
                        tmp = dist(ang, x, y,
                                   dead_car->corners[3].x, dead_car->corners[3].y,
                                   dead_car->corners[i].x, dead_car->corners[i].y);
                    }
                    else
                    {
                        tmp = dist(ang, x, y,
                                   dead_car->corners[i-1].x, dead_car->corners[i-1].y,
                                   dead_car->corners[i].x, dead_car->corners[i].y);
                    }
                    if(tmp < minimum)
                        minimum = tmp;
                }
            }
        }
        for(Car* car: cars) // samochody
        {
            if(this == car)
                continue;

            for(int i = 0; i < 4; i++)
            {
                if(i == 0)
                {
                    tmp = dist(ang, x, y,
                               car->car_borders->corners[3].x, car->car_borders->corners[3].y,
                               car->car_borders->corners[i].x, car->car_borders->corners[i].y);
                }
                else
                {
                    tmp = dist(ang, x, y,
                               car->car_borders->corners[i-1].x, car->car_borders->corners[i-1].y,
                               car->car_borders->corners[i].x, car->car_borders->corners[i].y);
                }
                if(tmp < minimum)
                    minimum = tmp;
            }
        }
        result.push_back(minimum);
    }// for(k)
    result[0] -= 0.5*width;
    result[1] -= sqrt(0.5*width * 0.5*width + 0.5*length * 0.5*length);
    result[2] -= 0.5*length;
    result[3] -= 0.5*length;
    result[4] -= 0.5*length;
    result[5] -= sqrt(0.5*width * 0.5*width + 0.5*length * 0.5*length);
    result[6] -= 0.5*width;
    return result;
}

bool Car::onRoad(Way &way)
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


    for(unsigned int i = 1; i < way.points.size(); i++)
    {
        Node A = way.points[i-1];
        Node B = way.points[i];

        if(car_borders->intersection(A, B)) // jesli przeciecie zwraca prawde
            return false;                   // to auto nie jest na drodze

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


