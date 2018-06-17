#pragma once
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
#include "processCommand.h"
#include <algorithm>
#include <memory>
#include "car.h"
#include "rectangle.h"
#include "mapElements.h"

using namespace std;
using namespace chrono;

#define PI 3.14159265

struct Node;
struct Way;
struct Road;
struct Rectangle;
class Car;

extern vector<Road*> all_roads;

double delta_t(system_clock::time_point bef);

double radToDeg(double rad);

double degToRad(double deg);

bool desc(int i, int j);

vector<int> vec_stoi(vector<string> x);

vector<double> vec_stod(vector<string> x);

template<typename Out>
inline void split(const string &s, char delim, Out result);

vector<string> split(const string &s, char delim);

template<typename T>
void push_back2(vector<T> &v, T &elem1, T &elem2);

double geo_vector(Node A, Node B, Node C);

bool check_inter(Node A, Node B, Node C);

double dist(double ang, double xG, double yG, double xA, double yA, double xB, double yB); // odległosc miedzy samochodem a odcinkiem
              //kat prostej,              auto(a,y) ,                    a(x,y)            i           b(x,y)

void setRoads(const string path);

Node moveNode(double x, double y, double a, double R);
