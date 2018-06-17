#include "processCommand.h"

using namespace std;
using namespace chrono;

void processCommand(const string &command)
{
    if(command == "")
        return;

    vector<string> parameters = split(command, ' ');

    if(parameters[0] == "newcar" && parameters.size() == 2)
    {
        newcar(stoi(parameters[1]));
        return;
    }
    if(parameters[0] == "movecar" && parameters.size() == 4)
    {
        movecar(stoi(parameters[1]), stod(parameters[2]), stod(parameters[3]));
        return;
    }
    if(parameters[0] == "rotatecar" && parameters.size() == 3)
    {
        rotatecar(stoi(parameters[1]), radToDeg(stod(parameters[2])));
        return;
    }
    if(parameters[0] == "killcar" && parameters.size() == 2)
    {
        killcar(stoi(parameters[1]));
        return;
    }
    if(parameters[0] == "setacceleration" && parameters.size() == 3)
    {
        switches[stoi(parameters[1])].first = stoi(parameters[2]);
        return;
    }
    if(parameters[0] == "setsteeringangle" && parameters.size() == 3)
    {
        switches[stoi(parameters[1])].second = stod(parameters[2]);
        return;
    }

    clog << "Received wrong command! \"" << command << "\"" << endl;
}

void processCommands(InputHandler &inputHandler)
{
    string command;
    while(inputHandler.getAvailableInput(command))
    {
        if(command == "pause") {
            for(;;)
            {
                inputHandler.waitForInput(command);
                if(command == "resume")
                {
                    start = system_clock::now();
                    return;
                }
                processCommand(command);
            }
        }
        else
        {
            processCommand(command);
        }
    }
}

void newcar(const int id)
{
    for(const int &car: cars_id)
    {
        if(id == car)
            return;
    }
    roads[12].cars.push_back(new Car(id, 1540, 350, 3.23, 0.315, 54, 0, 0, 0, 4.02, 1.7));

    cars_id.push_back(id);

    clog << "Car " << id << " added" << endl;
}

void movecar(const int id, const double x, const double y)
{
    for(Car* &car: roads[12].cars)
    {
        if(id == car->carId)
        {
            car->x = x;
            car->y = y;
            return;
        }
    }
}

void rotatecar(const int id, const double angle)
{
    for(Car* &car: roads[12].cars)
    {
        if(id == car->carId)
        {
            car->angle = angle;
            return;
        }
    }
}

void killcar(const int id)
{
    for(unsigned int i = 0; i < roads[12].cars.size(); i++)
    {
        if(id == roads[12].cars[i]->getId())
        {
            roads[12].broken_cars.push_back(new Rectangle(roads[12].cars[i]->car_borders->corners));

            roads[12].cars.erase(roads[12].cars.begin() + i, roads[12].cars.begin() + i + 1);
        }
    }
}


