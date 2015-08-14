/* 
 * Solves the Aliev-Panfilov model  using an explicit numerical scheme.
 * Based on code orginally provided by Xing Cai, Simula Research Laboratory
 * 
 * Modified and  restructured by Scott B. Baden, UCSD
 * 
 */

#include <assert.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <math.h>
#include "time.h"
#include "apf.h"
#include "Plotting.h"
#include <emmintrin.h>
#include <omp.h>

using namespace std;

void repNorms(double l2norm, double mx, double dt, int m,int n, int niter, int stats_freq);
double stats(double *E, int m, int n, double *_mx);


// Reports statistics about the computation: the L2 Norm and the Infinity NOrm
// These values should not vary (except to within roundoff)
// when we use different numbers of  processes to solve the problem


// The L2 norm of an array is computed by taking sum of the squares
// of each element, normalizing by dividing by the number of points
// and then taking the sequare root of the result
//
// The Linf norm is simply the maximum (absolute) value over
// all points in the array

__attribute__((optimize("no-tree-vectorize"))) // Prevent the compiler from vectorizing the code: it's your job to manually vectorize it
int solve(double **_E, double **_E_prev, double *R, int m, int n, int niters, double alpha, double dt, int plot_freq, Plotter *plotter, int stats_freq, bool debug){
	// Simulated time is different from the integer timestep number
	double t = 0.0;

	double *E = *_E, *E_prev = *_E_prev;
	double *R_tmp = R;
	double *E_tmp = *_E;
	double *E_prev_tmp = *_E_prev;
	int niter;

	int innerBlockRowStartIndex = (m+3)+1;
	int innerBlockRowEndIndex = (((m+3)*(n+3) - 1) - (m+1)) - (n+3);


	// We continue to sweep over the mesh until the simulation has reached
	// the desired simulation Time
	// This is different from the number of iterations

	for (niter = 0; niter < niters; niter++){

		if (debug){
			double mx;
			double l2norm = stats(E_prev,m,n,&mx);
			repNorms(l2norm,mx,dt,m,n,niter, stats_freq);
			if (plot_freq)
				plotter->updatePlot(E,  niter, m+1, n+1, WAIT);
		}

		/* 
		 * Copy data from boundary of the computational box to the
		 * padding region, set up for differencing computational box's boundary
		 *
		 * These are physical boundary conditions, and are not to be confused
		 * with ghost cells that we would use in an MPI implementation
		 *
		 * The reason why we copy boundary conditions is to avoid
		 * computing single sided differences at the boundaries
		 * which increase the running time of solve()
		 *
		 */


		// 4 FOR LOOPS for configuring the "paddings".
		int i,j;

		// Fills in the TOP Ghost Cells
		for (i = 0; i < (n+3); i++) {
			E_prev[i] = E_prev[i + (n+3)*2];
		}

		// Fills in the RIGHT Ghost Cells
		for (i = (n+2); i < (m+3)*(n+3); i+=(m+3)) {
			E_prev[i] = E_prev[i-2];
		}

		// Fills in the LEFT Ghost Cells
		for (i = 0; i < (m+3)*(n+3); i+=(m+3)) {
			E_prev[i] = E_prev[i+2];
		}	

		// Fills in the BOTTOM Ghost Cells
		for (i = ((m+3)*(n+3)-(m+3)); i < (m+3)*(n+3); i++) {
			E_prev[i] = E_prev[i - (n+3)*2];
		}

		//////////////////////////////////////////////////////////////////////////////

		// Solve for the excitation, a PDE
		for(j = innerBlockRowStartIndex; j <= innerBlockRowEndIndex; j+=(m+3)) {
			E_tmp = E + j;
			E_prev_tmp = E_prev + j;
			for(i = 0; i <= n; i++) {
				E_tmp[i] = E_prev_tmp[i]+alpha*(E_prev_tmp[i+1]+E_prev_tmp[i-1]-4*E_prev_tmp[i]+E_prev_tmp[i+(n+3)]+E_prev_tmp[i-(n+3)]);
			}
		}

		/* 
		 * Solve the ODE, advancing excitation and recovery variables
		 *     to the next timtestep
		 */
		for(j = innerBlockRowStartIndex; j <= innerBlockRowEndIndex; j+=(m+3)) {

			E_tmp = E + j;
			R_tmp = R + j;
			for(i = 0; i <= n; i++) {
				E_tmp[i] += -dt*(kk*E_tmp[i]*(E_tmp[i]-a)*(E_tmp[i]-1)+E_tmp[i]*R_tmp[i]);
				R_tmp[i] += dt*(epsilon+M1* R_tmp[i]/( E_tmp[i]+M2))*(-R_tmp[i]-kk*E_tmp[i]*(E_tmp[i]-b-1));
			}
		}

		//////////////////////////////////////////////////////////////////////////////////

		if (stats_freq){
			double mx;
			double l2norm = stats(E_prev,m,n,&mx);
			repNorms(l2norm,mx,dt,m,n,niter, stats_freq);
		}

		if (plot_freq){
			if (!(niter % plot_freq)){
				//            splot(E,niter,m+1,n+1,WAIT);
				plotter->updatePlot(E,  niter, m, n, WAIT);
			}
		}

		// Swap current and previous
		double *tmp = E; E = E_prev; E_prev = tmp;
	}	//end of 'niter' loop at the beginning

	// Store them into the pointers passed in

	*_E = E;
	*_E_prev = E_prev;

	return niter;
}

