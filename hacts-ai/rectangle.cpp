#include "rectangle.h"

using namespace std;

bool Rectangle::intersection(Node A, Node B)
{
    for(int i = 0; i < 4; i++)
    {
        Node C;
        Node D = corners[i];

        if(i == 0)
            C = corners[3];
        else
            C = corners[i-1];

        double 	v1 = geo_vector(C, D, A),
                v2 = geo_vector(C, D, B),
                v3 = geo_vector(A, B, C),
                v4 = geo_vector(A, B, D);

            // sprawdzanie czy odcinki sie przecinaja
        if((v1*v2 < 0 && v3*v4 < 0) || (((v1>0 && v2<0) || (v1<0 && v2>0)) && ((v3>0 && v4<0) || (v3<0 && v4>0))))
            return true;

            // sprawdzanie czy odcinki sie nie lacza ( czy skrajny punkt jednego nie nalezy do odcinka drugiego)
        if((v1 == 0 && check_inter(C, D, A))||
           (v2 == 0 && check_inter(C, D, B))||
           (v3 == 0 && check_inter(A, B, C))||
           (v4 == 0 && check_inter(A, B, D)))
        return true;
        // odcinki nie maja punktow wspolnych

    }
    return false;
}

bool Rectangle::intersection(Rectangle* &car)
{
    for(int j = 0; j < 4; j++)
    {
        Node A;
        Node B = car->corners[j];

        if(j == 0)
            A = car->corners[3];
        else
            A = car->corners[j-1];

        for(int i = 0; i < 4; i++)
        {
            Node C;
            Node D = corners[i];

            if(i == 0)
                C = corners[3];
            else
                C = corners[i-1];

            double 	v1 = geo_vector(C, D, A),
                    v2 = geo_vector(C, D, B),
                    v3 = geo_vector(A, B, C),
                    v4 = geo_vector(A, B, D);

                // sprawdzanie czy odcinki sie przecinaja
            if((v1*v2 < 0 && v3*v4 < 0) || (((v1>0 && v2<0) || (v1<0 && v2>0)) && ((v3>0 && v4<0) || (v3<0 && v4>0))))
                return true;

            // sprawdzanie czy odcinki sie nie lacza ( czy skrajny punkt jednego nie nalezy do odcinka drugiego)
            if((v1 == 0 && check_inter(C, D, A))||
               (v2 == 0 && check_inter(C, D, B))||
               (v3 == 0 && check_inter(A, B, C))||
              (v4 == 0 && check_inter(A, B, D)))
            return true;
           // odcinki nie maja punktow wspolnych

        }
    }
    return false;
}
