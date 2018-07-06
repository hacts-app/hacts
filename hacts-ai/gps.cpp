#include "gps.h"

using namespace std;

bool circle_intersection_with_stretch(Node A, Node B, Node S, double r)
{
    double closer_point_dist = min((A.x - S.x)*(A.x - S.x) + (A.y - S.y)*(A.y - S.y),
                              (B.x - S.x)*(B.x - S.x) + (B.y - S.y)*(B.y - S.y));       // odleglosc blizszego z punktow A i B

    double further_point_dist = max((A.x - S.x)*(A.x - S.x) + (A.y - S.y)*(A.y - S.y),
                               (B.x - S.x)*(B.x - S.x) + (B.y - S.y)*(B.y - S.y));      // odleglosc dalszego z punktow A i B

    if(closer_point_dist == r*r || further_point_dist == r*r) // jesli koniec odcinka lezy na obwodzie to
    {                                              //  odcinek ma punkt przeciecia z okregiem
        return true;
    }
    if(closer_point_dist < r*r && further_point_dist > r*r) // jesli 1 punkt jest wewnatrz kola a drugi nie to
    {                                            // odcinek ma punkt przeciecia z okregiem
        return true;
    }

    double a_stretch = (A.y - B.y) / (A.x - B.x); // kierunkowa prostej zawierajacej A i B

    double b_stretch = A.y - a_stretch * A.x; // punkt przeciecia tej prostej z OY

    double a = -1 / a_stretch; // kierunkowa prostej prostopadlej do  prostej zawierajacej A i B

    double b = S.y - a * S.x;  // punkt przeciecia z OY

    double x = (b - b_stretch) / (a_stretch - a); // x punktu przeciecia dwoch prostych

    double y = a * x + b; // y przeciecia dwoch prostych

    if(((x - S.x)*(x - S.x) + (y - S.y)*(y - S.y)) <= r*r) // jesli punkt przeciecia jest wew kola lub na obwodzie
    {
        if(check_inter(A, B, Node{x, y})) // jesli punkt przeciecia lezy na odcinku znaczy ze odcinek przecina okrag
        {
            return true;
        }
    }
    return false;
}

pair<long long, int> whereAmI(Car* car)
{
    Node S {car->get_x(), car->get_y()};      // srodek kola czyli wspolrzedne samochodu

    for(unsigned int i = 0; i < all_roads.size(); i++)
    {
        double r = all_roads[i]->lanes.size() * 2.5; // szerokosc drogi
        pair<bool, bool> intersection; // jedno na leftWay drugie na rightWay , punkt bedzie wewnatrz kiedy oba beda na true

        for(unsigned int j = 1; j < all_roads[i]->leftWay.points.size(); j++)
        {
            Node A = all_roads[i]->leftWay.points[j-1]; // A i B to dwa punkty ktore naleza do bandy drogi
            Node B = all_roads[i]->leftWay.points[j];

            if(circle_intersection_with_stretch(A, B, S, r))
            {
                intersection.first = true;
                break;
            }
        }

        for(unsigned int j = 1; j < all_roads[i]->rightWay.points.size(); j++)
        {
            Node A = all_roads[i]->rightWay.points[j-1]; // A i B to dwa punkty ktore naleza do bandy drogi
            Node B = all_roads[i]->rightWay.points[j];

            if(circle_intersection_with_stretch(A, B, S, r))
            {
                intersection.second = true;
                break;
            }
        }
        if(intersection.first && intersection.second) // znalezlismy road teraz pora na lane
        {
            r = 2.5; // teraz gdy szukamy po pasie szerokosc to 2.5 jak dla pasa ( niebezpieczenstwo przy wyliczeniach spowodowane ze szerokosc nie jest const 2.5)
            for(unsigned int j = 0; j < all_roads[i]->lanes.size(); j++)
            {
                intersection = {0,0}; // zerujemy wczesniej uzyte boole teraz dzialaja na tej samej zasadzie ale granice lane a nie road

                for(unsigned int k = 1; k < all_roads[i]->lanes[j].left_border.points.size(); k++)
                {
                    Node A = all_roads[i]->lanes[j].left_border.points[k-1]; // A i B to dwa punkty ktore naleza do bandy pasa
                    Node B = all_roads[i]->lanes[j].left_border.points[k];

                    if(circle_intersection_with_stretch(A, B, S, r))
                    {
                        intersection.first = true;
                        break;
                    }
                }
                                                    // ilosc punktow na (prawej) bandzie aktualnego pasa aktualnej drogi
                for(unsigned int k = 1; k < all_roads[i]->lanes[j].right_border.points.size(); k++)
                {
                    Node A = all_roads[i]->lanes[j].right_border.points[k-1]; // A i B to dwa punkty ktore naleza do bandy pasa
                    Node B = all_roads[i]->lanes[j].right_border.points[k];

                    if(circle_intersection_with_stretch(A, B, S, r))
                    {
                        intersection.second = true;
                        break;
                    }
                }

                if(intersection.first && intersection.second) // kiedy kolo ma stycznosc z 2 bandami oznacza to ze znalezlismy pas
                {
                    return {all_roads[i]->id, j}; // zwracamy id drogi oraz numer pasa czyli indeks pod ktorym znajduje sie pas na jakim sie znajdujemy
                }
            }
        }
    }
    return {-1, 0}; // domyslnie funkcja zwraca takie wartosci, oznacza to ze nie gps nie znalazl przynaleznosci do zadej drogi
}






