#pragma once
#include "functions.h"
#include "inputhandler.h"
#include "car.h"
#include <map>
#include <vector>

using namespace std;
using namespace chrono;

extern map<int, pair<int, double>> switches;
extern system_clock::time_point start;
extern vector<int> cars_id;

void processCommand(const string &command);

void processCommands(InputHandler &inputHandler);

void newcar(const int id);

void movecar(const int id, const double x, const double y);

void rotatecar(const int id, const double angle);

void killcar(const int id);

void setacceleration(const int id, const int action);
