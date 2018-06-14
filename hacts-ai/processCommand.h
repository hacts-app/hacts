#pragma once
#include "lib.h"
#include "inputhandler.h"
#include <map>
#include <vector>

using namespace std;
using namespace chrono;

extern system_clock::time_point start;

extern vector<int> cars_id;

void processCommand(const string &command);

void processCommands(InputHandler &inputHandler);

void newcar(const int id);

void movecar(const int id, const double x, const double y);

void rotatecar(const int id, const double angle);

void killcar(const int id);
