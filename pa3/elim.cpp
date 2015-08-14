/*
* Unblocked LU decomposition
* based on a parallel code written by
*  Anu Rao 11/1/94
*
* Code modifications made by 
* Scott B. Baden, UCSD, 5/3/15
*/
  
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <thread>
#include <functional> // ref
#include "cblock.h"
  
#include "barrier.h"
using namespace std;

//
// Globals
//

extern double **A, **R;
extern control_block cb;

//
// External Functions
//
double getTime();
   

// Globals
bool g_swap = false; 
int g_max = 0;

// Pivot Parallel Same as serial
void p_swap(double ** A, int k, int max, int lo, int hi) {
    int j;
    for ( j = lo; j < hi; j++ ){
	double t = A[max][j];
     	A[max][j] = A[k][j];
        A[k][j] = t;
    }
}
 
// Parallel Thread elim
void p_elim(int TID, int * sindex, int * eindex, int * ends,
	int pivot, int N, int NT, double ** A, barrier * barr) {

  int i, j, k;
  
  // Gaus Elimination Algorithm
  for ( k = 0; k < N; k++ ) {
    
     // Barrior Before Start
     barr->bsync(TID);
     
     // Have TID 0 Pivot
     if(TID == 0) {
   	
	g_max = k;
	if (pivot){ /* Partial Pivoting */
			
		for ( i = k+1; i < N; i++ ) {
            		if (fabs(A[i][k]) > fabs(A[g_max][k]))
                		g_max = i;
        	}
				
       } /* End Partial Pivoting */
    } 
    // Have TID 1 Load Balance
    else if(TID == 1) {
	int index, lo = k + 1;
	int itr = 0;
 
	int split = (N - (k + 1)) / NT;
	int rem = (N - (k + 1)) % NT;

	// Divide Splits
	for(index = 0; index < NT; index++){
		ends[index] = split;
	}

	// Divide left-overs
	while(rem) {
		ends[itr++] += 1;
		rem--;
		if(itr >= NT)
			itr = 0;
	}	

	// adjust start and end indexs
	for(index = 0; index < NT; index++) {
		sindex[index] = lo;
		eindex[index] = lo + ends[index];
		lo = eindex[index];
	}
    }

    // Barrier to Wait    
    barr->bsync(TID); 
    
    // Parallel swap pivot if enabled
    if(g_max > k) {
	if(!TID)
		p_swap(A, k, g_max, k, eindex[TID]);
	else
		p_swap(A, k, g_max, sindex[TID], eindex[TID]);
    }
    
    // Barrier after swapping
    barr->bsync(TID);

    // Threads can now elim row i,k
    double tmp,v = A[k][k];
    for ( i = sindex[TID]; i < eindex[TID]; i++ ) {
	tmp = A[i][k];
	tmp = tmp / v;
	A[i][k] = tmp;  
    }
 
    for ( i = sindex[TID]; i < eindex[TID]; i++ ) {
      	double Aik = A[i][k];
   	double *Ai = A[i];
      	for ( j = k+1; j < N; j++ ){
         	
		v = Ai[j];
		tmp = Aik * A[k][j];
		Ai[j] = v - tmp;
	
	}
    }  
  }
}

void elim()
{
  int i, j, k, Mx;

// If we get stuck, we can count the number of row swaps
// Do this for a small matrix
// int swaps = 0;
 
  // If num threads greater than or equal two, call parallelized version of elim
  if(cb.NT >= 2) {
	// Create a Barrier Object
	barrier barr(cb.NT);
	 	
	// Array of int to help divide work
	int ends[cb.NT];
	int sindex[cb.NT];
	int eindex[cb.NT];
	
	// Create Children Threads it Thread IDs
	thread tpool[cb.NT - 1];
	
	for(i = 0; i < (cb.NT - 1); i++)
		tpool[i] = thread(p_elim, i+1, &sindex[0], &eindex[0], &ends[0]
			,cb.partialPivoting, cb.N, cb.NT, A, &barr);

	// have parent call p_elim with with TID = 0
	p_elim(0, &sindex[0], &eindex[0], &ends[0],cb.partialPivoting
			, cb.N, cb.NT, A, &barr);
	
	// join threads at end
	for(i = 0; i < (cb.NT - 1); i++)
		tpool[i].join();
	
	return;
  }

  // Seriel Version of Gaus
  for ( k = 0; k < cb.N; k++ ) {
     if (cb.partialPivoting){ /* Partial Pivoting */
        Mx = k;
        for ( i = k+1; i < cb.N; i++ ) {
            if (fabs(A[i][k]) > fabs(A[Mx][k]))
                Mx = i;
        }
        if (Mx > k){
    //        swaps++;
            for ( j = k; j < cb.N; j++ ){
                double t = A[Mx][j];
                A[Mx][j] = A[k][j];
                A[k][j] = t;
            }
        }
      } /* End Partial Pivoting */


    for ( i = k+1; i < cb.N; i++ ) 
    	A[i][k] /= A[k][k];  

    for ( i = k+1; i < cb.N; i++ ) {
    	double Aik = A[i][k];
    	double *Ai = A[i];
    	for ( j = k+1; j < cb.N; j++ ) 
            Ai[j] -= Aik * A[k][j];
    }  
  }
}
