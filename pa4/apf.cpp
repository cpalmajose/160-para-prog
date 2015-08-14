/* 
 * Driver for a cardiac elecrophysioly simulatin that uses the
 * Aliev-Panfilov model
 * We use an explicit method
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
#include "Plotting.h"

using namespace std;


// Allocate a 1D flattened array

double *alloc1D(int m,int n){

    int nx=n+1, ny=m+1;
	double *E;
        // Wnsures that allocatd memory is aligned on a 16 by boundary
        assert(E= (double*)memalign(16, sizeof(double)*nx*ny));
//        int N_a = nx*ny* sizeof(double);
//        cout << "Na: " << N_a << endl;
//	assert(E =  aligned_alloc(16, N_a));  // align 16 at the beginning of the array

	return(E);
}

// External functions
void cmdLine(int argc, char *argv[], int& n, int& stats_freq, int& plot_freq, int &niters, bool &debug);
int solve(double **_E, double **_E_prev, double *R, int m, int n, int niters, double alpha, double dt, int plot_freq, Plotter *plotter, int stats_freq, bool debug);
void printTOD(string mesg);
void init (double *E,double *E_prev,double *R,int m,int n);
double stats(double *E, int m, int n, double *_mx);
void ReportStart(double dt, int niters, int m, int n);
void ReportEnd(int niters, double l2norm, double mx, int m,int n, double t0);
double stats(double *E, int m, int n, double *_mx);
double getTime();

// Main program
int main(int argc, char** argv)
{
 /*
  *  Solution arrays
  *   E is the "Excitation" variable, a voltage
  *   R is the "Recovery" variable
  *   E_prev is the Excitation variable for the previous timestep,
  *      and is used in time integration
  */
	double *E, *R, *E_prev;

 // Default values for the command line arguments
 int m=100,n=100;
 int stats_freq = 0;
 int plot_freq = 0;
 int niters=100;
 bool debug = false;

// Parse command line arguments
 cmdLine( argc, argv, n, stats_freq,  plot_freq, niters,debug);
 if (n < 26){
    cout << "\n *** N must be larger than 25.  Exiting ... " << endl << endl;
    exit(-1);
 }
 m = n;


 // Allocate contiguous memory for solution arrays
 // The computational box is defined on [1:m+1,1:n+1]
 // We pad the arrays in order to facilitate differencing on the 
 // boundaries of the computation box
 E = alloc1D(m+2,n+2);
 E_prev = alloc1D(m+2,n+2);
 R = alloc1D(m+2,n+2);

 init(E,E_prev,R,m,n);

 //
 // Initization of various simulation variables
 // Do not change the code these assignments statements, as doing so
 // could cause your submission to be graded incorrectly
 //

 // We compute the timestep dt

 // We should always use double precision values for the folowing variables:
 //    rp, dte, dtr, ddt
 //
 // This ensures that the computation of dte and especially dt
 // will not lose precision (i.e. if computed as single precision values)

 double dx = 1.0/n;
 double rp= kk*(b+1)*(b+1)/4;
 double dte=(dx*dx)/(d*4+((dx*dx))*(rp+kk));
 double dtr=1/(epsilon+((M1/M2)*rp));
 double ddt = (dte<dtr) ? 0.95*dte : 0.95*dtr;
 double dt = (double) ddt;
 double alpha = d*dt/(dx*dx);

 // End Initization of various simulation variables

 // Report various information
 // Do not remove this call, it is needed for grading
  ReportStart(dt, niters, m, n);

 Plotter *plotter = NULL;
 if (plot_freq){
     plotter = new Plotter();
     assert(plotter);
 }

 // Start the timer

 double t0 = -getTime();

 int niter = solve(&E, &E_prev, R, m, n, niters, alpha, dt, plot_freq, plotter, stats_freq, debug);

 t0 += getTime();

if (niter != niters)
   cout << "*** niters should be equal to niters" << endl;
 // Report various information
 // Do not remove this call, it is needed for grading
 double mx;
 double l2norm = stats(E_prev,m,n,&mx);
 ReportEnd(niters,l2norm,mx,m,n,t0);

 if (plot_freq){
    cout << "\n\nEnter any input to close the program and the plot...";
    int resp;
    cin >> resp;
  }

	 free (E);
	 free (E_prev);
	 free (R);

 if (plot_freq)
     delete plotter;
}
