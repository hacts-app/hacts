#pragma once
#include "functions.h"
#include "rectangle.h"
#include "mapElements.h"

using namespace std;

struct Node;
struct Way;
struct Rectangle;
struct Road;

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
    bool auto_drive;
public:
    Rectangle *car_borders;
    vector<Road*> roads;

public:
    Car(int id, int mass, double transfer, int torque, double radius, double max_velocity,
             double angle, double _x, double _y, double length, double width);

    void onGasPush(double trans, double delta);
    // trans od 0.00 do 1 to % wcisniecia gazu ... zakladamy ze aktywowane co sekunde

    void onBrakePush(double per, double delta);
       // tak jak w gazie

    void smoothChangeWheelAng(double position, double delta);
       // od -1 do 1 .. ujemne skrecaja w prawo dodatnie w lewo

    void quickChangeWheelAng(double position);

    void showPos(); // informacje dla nas

    void givePos(); // informacje dla ui

    vector<double> radar();
    // petla ktora liczy zblizone (pod stalymi katami) odleglosci od krawedzi drogi [POKI CO BRAK DETEKCJI INNYCH POJAZDOW]

    bool onRoad(Way &way);
    // sprawdza czy skrajne punkty (A,B,C,D) zawieraja sie w drodze po ktorej jedzie

    void changePos(double delta);
        // zmiana pozycji i KĄTA

    int getId() {return carId;}
    bool get_auto() {return auto_drive;}
    void set_auto(bool change) {auto_drive = change;}
    double getV() {return velocity;}
    double getWheelAng() {return wheelAng;}
    double getLen() {return length;}
    double getX() {return x;}
    double getY() {return y;}

    friend void movecar(const int id, const  double x, const  double y);
    friend void rotatecar(const int id, const double angle);
    friend void killcar(const int id);
};
