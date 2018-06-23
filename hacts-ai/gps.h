#include <iostream>
#include "mapElements.h"
#include "car.h"
#include "functions.h"

using namespace std;

class Car;
struct Node;
struct Way;
struct Lane;
struct Road;

extern vector<Road*> all_roads;

bool circle_intersection_with_stretch(Node A, Node B, Node S, double r); // true - odcinek przecina kolo

pair<long long, int> whereAmI(Car* car); // zwraca pierwsza liczbe jako id Road  a 2 to nr lane
