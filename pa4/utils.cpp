/* 
 * Utilities for the Aliev-Panfilov code
 *
 * Based on code orginally provided by Xing Cai, Simula Research Laboratory
 * 
 * Modified and  restructured by Scott B. Baden, UCSD
 */

#include <cstdlib>
#include <stdlib.h>
#include <malloc.h>
#include <fstream>
#include <iostream>
#include <assert.h>
#include <iomanip>
#include <string>
#include <math.h>
#include "apf.h"

using namespace std;

// Utilities
// 

void init (double *E,double *E_prev,double *R,int m,int n){
    int i;
    // Initialization

    for (i=0; i < (m+3)*(n+3); i++)
        E_prev[i] = R[i] = 0;

    for (i = (n+3); i < (m+2)*(n+3); i++) {
		int colIndex = i % (n+3);			// gives the base index (first row's) of the current index

		if(colIndex == 0 || colIndex == (n+2) || colIndex < (n/2+2))
			continue;

        E_prev[i] = 1.0;
	}

    for (i = 0; i < (m+3)*(n+3); i++) {
		int rowIndex = i / (n+3);			// gives the current row number in 2D array representation
		int colIndex = i % (n+3);			// gives the base index (first row's) of the current index

		if(colIndex == 0 || colIndex == (n+2) || rowIndex < (m/2+2))
			continue;

        R[i] = 1.0;
	}
}

double stats(double *E, int m, int n, double *_mx){
     double mx = -1;
     double l2norm = 0;
     int i, j;

     for (i=0; i< (m+3)*(n+3); i++) {

		int rowIndex = i / (n+3);			// gives the current row number in 2D array representation
		int colIndex = i % (n+3);		// gives the base index (first row's) of the current index		

		if(colIndex == 0 || colIndex == (n+2) || rowIndex == 0 || rowIndex == (m+2))
			continue;

	   l2norm += E[i]*E[i];
	   double fe = fabs(E[i]);
	   if (fe > mx)
	       mx = fe;
     }

// In the parallel version, you must sum all the local contributoins
// before dividing by (m+1)*(n+1)
     l2norm /= (double) ((m+1)*(n+1));
     l2norm = sqrt(l2norm);

     *_mx = mx;
     return l2norm;
 }
