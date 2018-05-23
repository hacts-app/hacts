#include <iostream>
#include "lib.h"

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

double dist(double ang, double xG, double yG, double xA, double yA, double xB, double yB)
{
    // wyliczanie prostej przechodzacej przez A i B
    double a = (yA - yB) / (xA - xB);

    double b = yA - a*xA;

    // wyliczanie prostej radarowej
    double aG = tan(ang * PI / 180.0);

    double bG = yG - aG*xG;

    if(a == aG) // jezeli sa rownolegle nie maja punktow stcznosci
        return 60;

    // x stycznosci:
    double x = (b - bG) / (aG - a);

    // czy x należy do dziedziny
    if((x >= xA && x <=xB) || (x >= xB && x <=xA))
    {
        return sqrt(pow((x - xG), 2) + pow((a*x + b - yG), 2)); // odleglosc auta od przeszkody
    }

    return 60; // max zasieg wzroku
}
/*
void setSectors(map<int,vector<Way*>> &sec, vector<Way> lin)
{
    for(unsigned int i = 0; i < lin.size(); i++)
    {
        vector<int> numOfSec;

        for(unsigned int j = 0; j < lin[i].points.size(); j++)
        {
            int numX = ceil((lin[i].points[j]->x)/30);

            int numY = ceil((lin[i].points[j]->y)/30);

            numOfSec.push_back((numY-1)*100 + numX);
        }

        sec[numOfSec[0]].push_back(new Way{vector<Node*>{new Node{lin[i].points[0]->x, lin[i].points[0]->y}}});
                                                    //points
        for(unsigned int j = 1; j < numOfSec.size(); j++)
        {
            if(numOfSec[j] == numOfSec[j-1])
            {
                sec[numOfSec[j]].back()->points.push_back(new Node{lin[i].points[j]->x, lin[i].points[j]->y});
            }
            else
            {
                // dodajemy stary punkt do nowego sektora
                sec[numOfSec[j]].push_back(new Way{vector<Node*>{new Node{lin[i].points[j-1]->x, lin[i].points[j-1]->y}}});

                // dodajemy nowy punkt do nowego sektora
                sec[numOfSec[j]].back()->points.push_back(new Node{lin[i].points[j]->x, lin[i].points[j]->y});

                // dodajemy nowy punkt do starego sektora
                sec[numOfSec[j-1]].back()->points.push_back(new Node{lin[i].points[j]->x, lin[i].points[j]->y});
            }
        }	// for j v2
    }	// for i
}

void setWay(map<int,vector<Way*>> &sec, string path)
{
    ifstream coord(path);
    string medium;
    vector<string> tmp;
    vector<Way> lines;
    int a = 0;

    while(coord>>medium)
    {
        tmp = split(medium, ',');

        for(unsigned int j = 0; j < tmp.size(); j++)
        {
            lines[a].points.push_back(new Node{stod(tmp[2 * j]), stod(tmp[2 * j + 1])});
        }
        a++;
    }
    setSectors(sec, lines);
}
*/
void setRoads(const string path)
{
    fstream file;
    string medium;
    vector<string> data;

    file.open(path, ios::in);

    while(getline(file, medium))
    {
        data.push_back(medium);
    }

    for(string x : data)
    {
        vector<string> tmp;

        tmp = split(x, '\t');

        // tmp contains {NameOfWay, NameOfRoad, NeighboursOfWay, NodesOfWay};

        int nr = stoi(tmp[1]);

        roads[nr].ways.push_back(Way{stoi(tmp[0])}); // dodajemy do drogi nowy Way ( jeszcze bez nodow)

        vector<string> tmpv= split(tmp[2], ',');
        vector<int> neighbrs;

        for(string z : tmpv)
        {
            neighbrs.push_back(stoi(z));
        }

        roads[nr].ways.back().neighboursId = neighbrs;

        vector<string> coords = split(tmp[3], ',');

        for(unsigned int i = 1; i < coords.size(); i++)
        {
            roads[nr].ways.back().points.push_back(new Node{stod(coords[i-1]), stod(coords[i])}); // dodawanie wszystkich wezlow do drogi
        }
    }
}

bool upOrDown(Node* &A, Node* &B, Node P)
{
    // wyliczanie prostej przechodzacej przez A i B
    double a = (A->y - B->y) / (A->x - B->x);

    double b = A->y - a * A->x;

    if(P.y > (a*P.x + b)) // powyzej
        return true;

    return false; // ponizej i na

}

Node moveNode(double x, double y, double a, double R)
{
    return Node{x + sin(a * PI / 180.0) * R, y +cos(a * PI / 180.0) * R};
}

Car::Car(int id, int m, double t, int tor, double r, double mv, double ang, double _x, double _y, double len, double wi)
              // masa, ,max moment silnika, przelozenie, promien kola, max predkosc
{
    carId = id;

    mass = m;

    max_transfer = t;

    torque = tor;

    radius = r;

    max_velocity = mv;

    angle = ang;

    x = _x;

    y = _y;

    velocity = 0;

    length = len;

    width = wi;

    wheelAng = 0;
}

void Car::onGasPush(double trans, clock_t &bef) // trans od 0.00 do 1 to % wciœniêcia gazu ... zak³adamy ¿e aktywowane co sekunde
{
    double transfer = max_transfer * trans;

    double F = (transfer * torque) / radius;

    acceleration = F / mass;

    velocity += (acceleration * delta_t(bef) * 0.001); // kom

    if(velocity > max_velocity)
        velocity = max_velocity;
}

void Car::onBrakePush(double per, clock_t &bef)
{
    const double max_acc = -9.59; // to chyba jest max opoznienie w hamowaniu

    acceleration = max_acc * per;

    velocity += (acceleration * delta_t(bef) * 0.001);  //kom

    if(velocity < 0)            // hamulec nie jest biegiem wstecznym
        velocity = 0;
}

void Car::changeWheelAng(double intensity, clock_t &bef)
{
    const int max_rot = 15 ; // maksymalna predkosc zmiany kata  kol w sekundzie

    double rot = max_rot * intensity;

    wheelAng += (rot * delta_t(bef) * 0.001);

    if(wheelAng > 40)
        wheelAng = 40;
    else if(wheelAng < -40)
        wheelAng = -40;
}

vector<double> Car::radar(vector<Way*> &ways)
{
    vector<double> result;
    double minimum = 60;

    for(int k = 0; k < 14; k++)
    {
        double ang;

        switch(k)
        {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
                {
                    ang = angle - (k * 15.0);
                }break;
            case 7:
                {
                    ang = angle - 180.0;
                }break;
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
                {
                    ang = angle - (270 + (k-8)*15.0);
                }
        }

        for(unsigned int i = 0; i < ways.size(); i++)
        {
            for(unsigned int j = 1; j < ways[i]->points.size(); j++)
            {
                double xA = ways[i]->points[j-1]->x;
                double yA = ways[i]->points[j-1]->y;

                double xB = ways[i]->points[j]->x;
                double yB = ways[i]->points[j]->y;

                double tmp = dist(ang, x, y, xA, yA, xB, yB);

                if(tmp < minimum)
                    minimum = tmp;
            }
        }
        result.push_back(minimum);
    }
    return result;
}

bool Car::onRoad(vector<Node*> &hiWay, vector<Node*> &loWay)
{
    double R = (sqrt(pow(length, 2) + pow(width, 2))) / 2;
    // promien okregu opisanego na samochodzie

    double alpha = acos(1-(pow(width, 2)) / (2 * pow(R, 2))) * 180.0 / PI;
    // kat miedzy dwoma R-ami naprzeciwko szerokosci pojazdu

    double a1 = tan((angle + alpha/2) * PI / 180.0);
    // kierunkowa 1 R

    double a2 = tan((angle - alpha/2) * PI / 180.0);
    // kierunkowa 2 R

    vector<Node> carCorners; // ABCD ...

    carCorners.push_back(moveNode(x, y, a1, R));

    carCorners.push_back(moveNode(x, y, a2, R));

    carCorners.push_back(moveNode(x, y, a1, -R));

    carCorners.push_back(moveNode(x, y, a2, -R));

    for(Node i : carCorners)
    {
        bool firstVerif;
        bool secondVerif;

        for(unsigned int j = 1; j < loWay.size(); j++)
        {
            // szukanie 2 nodow miedzy ktorymi sie znajduje dany punkt
            if((i.x >= loWay[j]->x && i.x <= loWay[j-1]->x) || (i.x >= loWay[j-1]->x && i.x <= loWay[j]->x))
            {
                firstVerif = upOrDown(loWay[j], loWay[j-1], i);

                break;
            }
        } // loWay

        for(unsigned int j = 1; j < hiWay.size(); j++)
        {
            // szukanie 2 nodow miedzy ktorymi sie znajduje dany punkt
            if((i.x >= hiWay[j]->x && i.x <= hiWay[j-1]->x) || (i.x >= hiWay[j-1]->x && i.x <= hiWay[j]->x))
            {
                secondVerif = upOrDown(hiWay[j], hiWay[j-1], i);

                break;
            }
        } // hiWay

        if(firstVerif == secondVerif) // jesli samochod bedzie pod lub nad obiema krawedziami naraz to wyjechal
            return false;
    }

    return true;
}

void Car::showPos()
{
    clog << "(x = " << x << ", y = " << y <<") Angle = " << angle << " V = " << velocity <<endl;
}

void Car::givePos()
{
    cout << carId <<" "<< x <<" "<< y <<" "<<angle <<endl;
}

void Car::changePos(clock_t &bef)
{
    vector<clock_t> tmp(3, bef);

    angle += radToDeg( (2 * velocity * sin(wheelAng * PI / 180.0)) / length ) * delta_t(tmp[0]) * 0.001;

    if(angle > 360)
        angle -= 360;

    if(angle < 0)
        angle += 360;

    x += (velocity * cos(angle * PI / 180.0)  * delta_t(tmp[1]) * 0.001);

    y += (velocity * sin(angle * PI / 180.0)  * delta_t(tmp[2]) * 0.001 );

    bef = clock();

}
