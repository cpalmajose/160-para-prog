#ifndef __PLOTTING_H
#define __PLOTTING_H
#include <stdlib.h>
#include <cstring>
#include <string>
#include <cstdio>
using namespace std;

class Plotter {
public:
    Plotter();
    ~Plotter();
    void Plot(int **image, int x, int y, string& cLine);


private:
    FILE *gnu_pipe;
};

#endif
