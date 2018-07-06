#pragma once
#include "functions.h"
#include "rectangle.h"
#include "mapElements.h"

using namespace std;

struct Node;
struct Way;
struct Rectangle;
struct Lane;
struct Road;

class Car
{
private:
    int carId;
    double velocity;
    double max_velocity;
    int mass;
    double max_transfer;
    double torque;
    double radius;
    double angle;
    double wheelAng;
    double x,y;
    double length;
    double width;
    bool auto_drive;
    unsigned int lane_id;
    vector<Lane*> lanes;
    Rectangle *car_borders; 
    vector<Road*> roads;

public:
    Car(int carId, double length, double width);

    void onGasPush(double trans, double delta);
    // trans od 0.00 do 1 to % wcisniecia gazu ...

    void onBrakePush(double per, double delta);
       // tak jak w gazie

    void smoothChangeWheelAng(double position, double delta); // obrot kierownica o wartosci
       // od -1 do 1 .. ujemne skrecaja w prawo dodatnie w lewo

    void quickChangeWheelAng(double position); // obrot kierownica DO wartosci (blyskawiczne)

    void showPos(); // informacje dla nas

    void givePos(); // informacje dla ui

    bool onRoad(Way &way);
    // sprawdza czy skrajne punkty (A,B,C,D) zawieraja sie w drodze po ktorej jedzie

    void changePos(double delta);
        // zmiana pozycji , kata

    vector<Node> set_corners(); // wyliczanie 4 naroznikow samochodu

    vector<double> radar();
    // petla ktora liczy zblizone (pod stalymi katami) odleglosci od krawedzi drogi

    void common_change_road();

    // gettery

    int get_carId();

    double get_velocity();

    double get_angle();

    double get_wheelAng();

    double get_x();

    double get_y();

    bool get_auto_drive();

    unsigned int get_lane_id();

    vector<Lane *> get_lanes();

    Rectangle* get_car_borders();

    vector<Road*> get_roads();

    // settery

    void set_max_velocity(double value);

    void set_mass(int value);

    void set_max_transfer(double value);

    void set_torque(double value);

    void set_radius(double value);

    void set_angle(double value);

    void set_x(double value);

    void set_y(double value);

    void set_auto_drive(bool value);

    void set_lane_id(unsigned int value);

    void set_lanes(vector<Lane *> pointers);

    void set_car_borders(Rectangle* pointer); // potrzeba ?

    void set_roads(vector<Road*> pointers);

};
