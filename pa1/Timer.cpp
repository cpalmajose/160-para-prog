//
// Do not modify this file
//
#include <sys/time.h>
#include <iostream>
using namespace std;
const double kMicro = 1.0e-6;
double getTime()
{
    struct timeval TV;

    const int RC = gettimeofday(&TV, NULL);
    if(RC == -1)
    {
        cerr << "ERROR: Bad call to gettimeofday\n";
        return(-1);
    }

    return( ((double)TV.tv_sec) + kMicro * ((double)TV.tv_usec) );

}  // end getTime()
