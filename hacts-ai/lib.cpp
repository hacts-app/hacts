#include <iostream>
#include "lib.h"

using namespace std;
using namespace chrono;

double delta_t(system_clock::time_point bef)
{
    system_clock::time_point now = system_clock::now();
    return (now - bef).count() / 1000000000.0; /// ???
}

double radToDeg(double rad)
{
    return (180.0 * rad) / PI;
}

double degToRad(double deg)
{
    return (PI * deg) / 180.0;
}

vector<int> vec_stoi(vector<string> x)
{
    vector<int> loc;

    for(string element: x)
    {
        loc.push_back(stoi(element));
    }
    return loc;
}

vector<double> vec_stod(vector<string> x)
{
    vector<double> loc;

    for(string element: x)
    {
        loc.push_back(stod(element));
    }
    return loc;
}

template<typename Out>
inline void split(const string &s, char delim, Out result)
{
    stringstream ss(s);
    string item;

    while (getline(ss, item, delim))
    {
        *(result++) = item;
    }
}

vector<string> split(const string &s, char delim)
{
    vector<string> elems;
    split(s, delim, back_inserter(elems));

    return elems;
}

template<typename T>
void push_back2(vector<T> &v, T &elem1, T &elem2)
{
    v.push_back(&elem1);
    v.push_back(&elem2);
}

double geo_vector(Node A, Node B, Node C) // tworzenie iloczynu wektorowego
{
    double 	x1 = C.x - A.x,
            y1 = C.y - A.y,
            x2 = B.x - A.x,
            y2 = B.y - A.y;
    return x1*y2 - x2*y1;
}

bool check_inter(Node A, Node B, Node C) // sprawdzanie czy krawedz nie nalezy do drugiego odcinka
{
    if(min(A.x, B.x) <= C.x && C.x <= max(A.x, B.x) &&
       min(A.y, B.y) <= C.y && C.y <= max(A.y, B.y))
        return true;
    return false;
}

static inline float fastTan(const float a) {
        return sin(a) / cos(a);
}

double dist(double ang, double xG, double yG, double xA, double yA, double xB, double yB)
{
    // wyliczanie prostej przechodzacej przez A i B
    double a = (yA - yB) / (xA - xB);

    double b = yA - a*xA;

    // wyliczanie prostej radarowej
    double aG = fastTan(ang * PI / 180.0);

    double bG = yG - aG*xG;

    if(a == aG) // jezeli sa rownolegle nie maja punktow stcznosci
        return 60;

    // x stycznosci:
    double x = (b - bG) / (aG - a);
    // policz y
    double y = a*x + b;

    //czy x nalezy do dziedziny polprostej
    if((ang >= 0 && ang < 90) || (ang > 270 && ang < 360)) // x polprostej -> inf
    {
        if(x < xG) // jesli punkt jest na lewo to nie ma
            return 60;
    }
    else if((ang > 90 && ang <= 180) || (ang > 180 && ang < 270)) // x polprostej -> -inf
    {
        if(x > xG)
            return 60;
    }
    else
    {

        if(ang == 90 && y < yG)
            return 60;

        if(ang == 270 && y > yG)
            return 60;
    }

    // czy x należy do dziedziny krawedzi
    if((x >= xA && x <=xB) || (x >= xB && x <=xA))
    {
        return sqrt((x - xG)*(x - xG) + (y - yG)*(y - yG)); // odleglosc auta od przeszkody
    }

    return 60; // max zasieg wzroku
}

void setRoads(const string path)
{
    fstream file;
    string medium;

    file.open(path, ios::in);

    while(getline(file, medium))
    {
        vector<string> elements;

        elements = split(medium, '\t');

        int nr = stoi(elements[1]);

        roads[nr].ways.push_back(Way{stoi(elements[0])}); // dodajemy do drogi nowy Way ( jeszcze bez nodow)

        vector<int> neighbours= vec_stoi(split(elements[2], ','));

        roads[nr].ways.back().neighboursId = neighbours;

        vector<double> coords = vec_stod(split(elements[3], ','));

        for(unsigned int i = 1; i < coords.size(); i+=2)
        {
            roads[nr].ways.back().points.push_back(Node{coords[i-1], coords[i]}); // dodawanie wszystkich wezlow do drogi
        }
    }
}

Node moveNode(double x, double y, double a, double R)
{
    return Node{x + sin(a * PI / 180.0) * R, y +cos(a * PI / 180.0) * R};
}

Car::Car(int id, int mass, double transfer, int torque, double radius, double max_velocity,
         double angle, double _x, double _y, double length, double width)
    : car_borders({Node{0,0}})
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

    double a1 = tan((angle + alpha/2) * PI / 180.0);
    // kierunkowa 1 R

    double a2 = tan((angle - alpha/2) * PI / 180.0);
    // kierunkowa 2 R

    vector<Node> carCorners{
        moveNode(x, y, a1, R),
        moveNode(x, y, a2, R),
        moveNode(x, y, a1, -R),
        moveNode(x, y, a2, -R),
    }; // ABCD ...

    car_borders.update(carCorners);
}

void Car::onGasPush(double trans, system_clock::time_point bef) // trans od 0.00 do 1 to % wciœniêcia gazu ... zak³adamy ¿e aktywowane co sekunde
{
    double transfer = max_transfer * trans;

    double F = (transfer * torque) / radius;

    acceleration = F / mass;

    velocity += (acceleration * delta_t(bef));

    if(velocity > max_velocity)
        velocity = max_velocity;
}

void Car::onBrakePush(double per, system_clock::time_point bef)
{
    const double max_acc = -9.59; // to chyba jest max opoznienie w hamowaniu

    acceleration = max_acc * per;

    velocity += (acceleration * delta_t(bef));

    if(velocity < 0)            // hamulec nie jest biegiem wstecznym
        velocity = 0;
}

void Car::changeWheelAng(double intensity, system_clock::time_point bef)
{
    const int max_rot = 15 ; // maksymalna predkosc zmiany kata  kol w sekundzie

    double rot = max_rot * intensity;

    wheelAng += (rot * delta_t(bef));

    if(wheelAng > 40)
        wheelAng = 40;
    else if(wheelAng < -40)
        wheelAng = -40;
}

vector<double> Car::radar(vector<Way> &ways)
{
    vector<double> result;
    double minimum = 60;

    for(int k = 0; k < 14; k++)
    {
        double ang;

        if(k < 7)
            ang = angle + (k * 15.0);
        else if(k == 7)
            ang = angle + 180.0;
        else
            ang = angle + (270 + (k-8) * 15.0);

        //while(ang >= 360)
        //    ang-=360;

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

        minimum = 60;
    }
    return result;
}

bool Car::onRoad(vector<Way> &ways)
{
    double R = (sqrt(pow(length, 2) + pow(width, 2))) / 2;
    // promien okregu opisanego na samochodzie

    double alpha = acos(1-(pow(width, 2)) / (2 * pow(R, 2))) * 180.0 / PI;
    // kat miedzy dwoma R-ami naprzeciwko szerokosci pojazdu

    double a1 = tan((angle + alpha/2) * PI / 180.0);
    // kierunkowa 1 R

    double a2 = tan((angle - alpha/2) * PI / 180.0);
    // kierunkowa 2 R

    vector<Node> carCorners{
        moveNode(x, y, a1, R),
        moveNode(x, y, a2, R),
        moveNode(x, y, a1, -R),
        moveNode(x, y, a2, -R),
    }; // ABCD ...

    car_borders.update(carCorners);

    for(const Way &way: ways)
    {
        for(int i = 1; i < way.points.size(); i++)
        {
            Node A = way.points[i-1];
            Node B = way.points[i];

            if(car_borders.intersection(A, B)) // jesli przeciecie zwraca prawde
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

void Car::changePos(system_clock::time_point bef)
{
    angle += radToDeg( (2 * velocity * sin(wheelAng * PI / 180.0)) / (length-0.6) ) * delta_t(bef);

    if(angle > 360)
        angle -= 360;

    if(angle < 0)
        angle += 360;

    x += (velocity * cos((angle) * PI / 180.0) * delta_t(bef));

    y += (velocity * sin((angle) * PI / 180.0) * delta_t(bef));
}

bool Rectangle::intersection(Node A, Node B)
{
    for(int i = 1; i < 4; i++)
    {
        Node C = corners[i-1];
        Node D = corners[i];
        double 	v1 = geo_vector(C, D, A),
                v2 = geo_vector(C, D, B),
                v3 = geo_vector(A, B, C),
                v4 = geo_vector(A, B, D);

            // sprawdzanie czy odcinki sie przecinaja
        if((v1*v2 < 0 && v3*v4 < 0) || (((v1>0 && v2<0) || (v1<0 && v2>0)) && ((v3>0 && v4<0) || (v3<0 && v4>0))))
            return true;

            // sprawdzanie czy odcinki sie nie lacza ( czy skrajny punkt jednego nie nalezy do odcinka drugiego)
        if((v1 == 0 && check_inter(C, D, A))||
           (v2 == 0 && check_inter(C, D, B))||
           (v3 == 0 && check_inter(A, B, C))||
           (v4 == 0 && check_inter(A, B, D)))
        return true;
        // odcinki nie maja punktow wspolnych

    }
    return false;
}
