#include <iostream>
#include "Plotter.h"
using namespace std;


Plotter::Plotter() {
    gnu_pipe = popen("gnuplot -persist","w");
}

void Plotter::Plot(int **image, int x, int y, string& cLine){
  auto mx= -1, mn = 32768;
  for (auto j=0; j<y; j++)
     for (auto i=0; i<x; i++){
        if (image[j][i] > mx)
            mx = image[j][i];
        if (image[j][i] < mn)
            mn = image[j][i];
  }
  fprintf(gnu_pipe,"\n");
  fprintf(gnu_pipe,"\n");
  fprintf(gnu_pipe,"set size square\n");
  fprintf(gnu_pipe,"set key off\n");
  fprintf(gnu_pipe,"set pm3d map\n");
  fprintf(gnu_pipe,"set palette defined (-3 \"blue\", 0 \"white\", 1 \"red\")\n");
  char * cstr = new char [cLine.length()+1];
  std::strcpy (cstr, cLine.c_str());
  fprintf(gnu_pipe, "set title \"%s\"\n",cstr);


  fprintf(gnu_pipe,"splot [0:%d] [0:%d][%d:%d] \"-\"\n",x-1,y-1,mn,mx);
  for (auto j=0; j<y; j++){
       for (auto i=0; i<x; i++)
           fprintf(gnu_pipe,"%d %d %d\n", i, j, image[j][i]);
       fprintf(gnu_pipe,"\n");
  }
  fprintf(gnu_pipe,"e\n");
  fflush(gnu_pipe);
  delete[] cstr;
  return;
}

Plotter::~Plotter() {
    pclose(gnu_pipe);
}

