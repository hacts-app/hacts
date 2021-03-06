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
    if(parameters[0] == "setautopilot" && parameters.size() == 3)
    {
        setautopilot(stoi(parameters[1]), stoi(parameters[2]));
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
    cars.push_back(new Car(id, 4.02, 1.7));

    cars.back()->set_mass(1540);

    cars.back()->set_max_transfer(350);

    cars.back()->set_torque(3.23);

    cars.back()->set_radius(0.315);

    cars.back()->set_max_velocity(54);

    cars.back()->set_roads(all_roads);

    cars_id.push_back(id);

    clog << "Car " << id << " added" << endl;
}

void movecar(const int id, const double x, const double y)
{
    for(Car* car: cars)
    {
        if(id == car->get_carId())
        {
            car->set_x(x);
            car->set_y(y);
            return;
        }
    }
}

void rotatecar(const int id, const double angle)
{
    for(Car* car: cars)
    {
        if(id == car->get_carId())
        {
            car->set_angle(angle);
            return;
        }
    }
}

void killcar(const int id)
{
    for(unsigned int i = 0; i < cars.size(); i++)
    {
        if(id == cars[i]->get_carId())
        {
            for(Road* road: cars[i]->get_roads())
            {
                road->broken_cars.push_back(new Rectangle(cars[i]->get_car_borders()->corners));

                cars.erase(cars.begin() + i, cars.begin() + i + 1);
            }
        }
    }
}

void setautopilot(int id, int _switch)
{
    for(Car* car: cars)
    {
        if(id == car->get_carId())
        {
            car->set_auto_drive(_switch);
            return;
        }
    }
}

