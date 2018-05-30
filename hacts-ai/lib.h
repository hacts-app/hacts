#include <iostream>
#include <ctime>
#include <vector>
#include <thread>
#include <chrono>
#include <iomanip>
#include <cmath>
#include <sstream>
#include <iterator>
#include <fstream>
#include <string>
#include <map>


using namespace std;
using namespace chrono;

#define PI 3.14159265

class Car;
struct Node;
struct Way;
struct Road;

extern map<int, Road> roads;

int delta_t(system_clock::time_point bef);

double radToDeg(double rad);

double degToRad(double deg);

double dist(double ang, double xG, double yG, double xA, double yA, double xB, double yB); // odległosc miedzy samochodem a odcinkiem
              //kat prostej,              auto(a,y) ,                    a(x,y)            i           b(x,y)

template<typename Out>
inline void split(const string &s, char delim, Out result);

vector<string> split(const string &s, char delim);

template<typename T>
void push_back2(vector<T> &v, T &elem1, T &elem2);
/*
void setSectors(map<int,vector<Way*>> &sec, vector<Way> lin);

void setWay(map<int,vector<Way*>> &sec, string path);
*/
void setRoads(const string path);

bool upOrDown(Node* &A, Node* &B, Node P);

Node moveNode(double x, double y, double a, double R);

class Car
{
private:
    int carId;
    double velocity;
    double max_velocity;
    double acceleration;
    int mass;
    double max_transfer;
    int torque;
    double radius;
    double angle;
    double wheelAng;
    double x,y;
    double length;
    double width;

public:
    Car(int id, int m, double t, int tor, double r, double mv, double ang, double _x, double _y, double len, double wi);
    // id, masa, max moment silnika, przelozenie, promien kola, max predkosc

    void onGasPush(double trans, system_clock::time_point bef);
    // trans od 0.00 do 1 to % wcisniecia gazu ... zakladamy ze aktywowane co sekunde

    void onBrakePush(double per, system_clock::time_point bef);
       // tak jak w gazie

    void changeWheelAng(double intensity, system_clock::time_point bef);
       // od -1 do 1 .. ujemne skrecaja w prawo dodatnie w lewo

    void showPos(); // informacje dla nas

    void givePos(); // informacje dla ui

    vector<double> radar(vector<Way> &ways);
    // petla ktora liczy zblizone (pod stalymi katami) odleglosci od krawedzi drogi [POKI CO BRAK DETEKCJI INNYCH POJAZDOW]

    bool onRoad(vector<Node*> &hiWay, vector<Node*> &loWay);
    // sprawdza czy skrajne punkty (A,B,C,D) zawieraja sie w drodze po ktorej jedzie

    void changePos(system_clock::time_point bef);
        // zmiana pozycji i KĄTA

    void AngTo(double q) {angle = q;}
    void WAngTo(double q){wheelAng = q;}

    double getV() {return velocity;}
    double getX() {return x;}
    double getY() {return y;}
    double getAng() {return angle;}
    double getWAng() {return wheelAng;}

};

struct Node
{
    double x;
    double y;
};

struct Way
{
    int id;
    vector<int> neighboursId;
    vector<Node*> points;
};

struct Road
{
    vector<Way> ways;
};
