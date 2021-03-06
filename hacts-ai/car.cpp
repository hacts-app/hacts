#include "car.h"
#include "rectangle.h"

using namespace std;

Car::Car(int carId, double length, double width)
{
    this->carId = carId;
    this->length = length;
    this->width = width;

    angle = 0;
    x = 0;
    y = 0;
    velocity = 0;
    wheelAng = 0;
    auto_drive = true;

    car_borders = new Rectangle(set_corners());
}

void Car::onGasPush(double trans, double delta) // trans od 0.00 do 1 to % wciœniêcia gazu ... zak³adamy ¿e aktywowane co sekunde
{
    double transfer = max_transfer * trans;

    double F = (transfer * torque) / radius;

    double acceleration = F / mass;

    velocity += acceleration * delta;

    if(velocity > max_velocity)
        velocity = max_velocity;
}

void Car::onBrakePush(double per, double delta)
{
    const double max_acc = -9.59; // max opoznienie w hamowaniu

    double acceleration = max_acc * per;

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



bool Car::onRoad(Way &way)
{
    car_borders->update(set_corners());

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

vector<Node> Car::set_corners()
{
    double R = (sqrt(pow(length, 2) + pow(width, 2))) / 2;
    // promien okregu opisanego na samochodzie

    double alpha = acos(1-(pow(width, 2)) / (2 * pow(R, 2))) * 180.0 / PI;
    // kat miedzy dwoma R-ami naprzeciwko szerokosci pojazdu

    double a1 = angle + alpha/2;
    // kat prostej 1R

    double a2 = angle - alpha/2;
    // kat prostej 2R

    return {
        moveNode(x, y, a1, R),
        moveNode(x, y, a2, R),
        moveNode(x, y, a1, -R),
        moveNode(x, y, a2, -R),
    }; // ABCD ...
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

        for(Lane* lane: lanes) // zbadaj kazdy pas na tej drodze
        {
            for(unsigned int j = 1; j < lane->left_border.points.size(); j++)
            {
                tmp = dist(ang, x, y,
                           lane->left_border.points[j-1].x, lane->left_border.points[j-1].y,
                           lane->left_border.points[j].x, lane->left_border.points[j].y);

                if(tmp < minimum)
                    minimum = tmp;
            }
            for(unsigned int j = 1; j < lane->right_border.points.size(); j++)
            {
                tmp = dist(ang, x, y,
                           lane->right_border.points[j-1].x, lane->right_border.points[j-1].y,
                           lane->right_border.points[j].x, lane->right_border.points[j].y);

                if(tmp < minimum)
                    minimum = tmp;
            }

        }
        for(Road* road: roads)
        {
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
        for(Car* car: cars) // samochody ( wszystkie )
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

void Car::common_change_road()
{
    Node A = lanes[0]->left_border.points.back(); // dwa skrajne punkty aktualnego pasa
    Node B = lanes[0]->right_border.points.back(); // laczymy i sprawdzamy przeciecie ktore skutkuje zmiane drogi

    if(car_borders->intersection(A, B))
    {
        if(lanes[1]->left_border.points.front().compare(A) || lanes[1]->right_border.points.front().compare(A) ||
           lanes[1]->left_border.points.back().compare(A)  || lanes[1]->right_border.points.back().compare(A))    // szukamy czy to na lane [2] wiechalo auto
        {
             // lane[1] jest teraz lane 0, Road[1] jest Road[0] nr_lane nie zmienia sie

          //  swap(roads[0], roads[num]);
          //  swap(lanes[0], lanes[num]);

        }


    }
}

// gettery

int Car::get_carId() {
    return carId;
}

double Car::get_velocity() {
    return velocity;
}

double Car::get_angle() {
    return angle;
}

double Car::get_wheelAng() {
    return wheelAng;
}

double Car::get_x() {
    return x;
}

double Car::get_y() {
    return y;
}

bool Car::get_auto_drive() {
    return auto_drive;
}

unsigned int Car::get_lane_id() {
    return lane_id;
}

vector<Lane*> Car::get_lanes() {
    return lanes;
}

Rectangle* Car::get_car_borders() {
    return car_borders;
}

vector<Road*> Car::get_roads() {
    return roads;
}

// settery

void Car::set_max_velocity(double value) {
    max_velocity = value;
}

void Car::set_mass(int value) {
    mass = value;
}

void Car::set_max_transfer(double value) {
    max_transfer = value;
}

void Car::set_torque(double value) {
    torque = value;
}

void Car::set_radius(double value) {
    radius = value;
}

void Car::set_angle(double value) {
    angle = value;
}

void Car::set_x(double value) {
    x = value;
}

void Car::set_y(double value) {
    y = value;
}

void Car::set_auto_drive(bool value) {
    auto_drive = value;
}

void Car::set_lane_id(unsigned int value) {
    lane_id = value;
}

void Car::set_lanes(vector<Lane*> pointers) {
    lanes = pointers;
}

void Car::set_car_borders(Rectangle* pointer) { // potrzeba ?
    car_borders = pointer;
}

void Car::set_roads(vector<Road*> pointers) {
    roads = pointers;
}
