#include <iostream>
#include "functions.h"

using namespace std;
using namespace chrono;

double delta_t(system_clock::time_point bef)
{
    system_clock::time_point now = system_clock::now();

    typedef duration<double> sec;

    sec delta = now - bef;

    return delta.count();
}

double radToDeg(double rad)
{
    return (180.0 * rad) / PI;
}

double degToRad(double deg)
{
    return (PI * deg) / 180.0;
}

bool desc(int i, int j) {return i > j;}

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
{                                   // liczenie wartosci na osi Z (x i y = 0 zawsze)
    double 	x1 = C.x - A.x, //      1 i 2 to wektory CA i BA
            y1 = C.y - A.y, //      z macierzy :
            x2 = B.x - A.x, //   1   | 1x  1y|
            y2 = B.y - A.y; //   2   | 2x  2y|  =  1x*2y - 1y*2x
    return x1*y2 - x2*y1;   //                   (mnozymy na krzyz obliczajac z)
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
        return 100;

    // x stycznosci:
    double x = (b - bG) / (aG - a);
    // policz y
    double y = a*x + b;

    //czy x nalezy do dziedziny polprostej
    if((ang >= 0 && ang < 90) || (ang > 270 && ang < 360)) // x polprostej -> inf
    {
        if(x < xG) // jesli punkt jest na lewo to nie ma
            return 100;
    }
    else if((ang > 90 && ang <= 180) || (ang > 180 && ang < 270)) // x polprostej -> -inf
    {
        if(x > xG)
            return 100;
    }
    else
    {

        if(ang == 90 && y < yG)
            return 100;

        if(ang == 270 && y > yG)
            return 100;
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

        vector<int> neighbours= vec_stoi(split(elements[2], ','));

        roads[nr].ways.push_back(Way{stoi(elements[0]) ,neighbours}); // dodajemy do drogi nowy Way ( jeszcze bez nodow)

        vector<double> coords = vec_stod(split(elements[3], ','));

        for(unsigned int i = 1; i < coords.size(); i+=2)
        {
            roads[nr].ways.back().points.push_back(Node{coords[i-1], coords[i]}); // dodawanie wszystkich wezlow do drogi
        }
    }
}

Node moveNode(double x, double y, double a, double R)
{
    return Node{x + cos(a * PI / 180.0) * R, y + sin(a * PI / 180.0) * R};
}


