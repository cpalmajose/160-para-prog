#include <iostream>
#include <assert.h>
#include <cstdlib>
#include <thread>
using namespace std;


// Point computation (serial)
// Implemented in smdb.h
int ComputeMandelbrotPoint(int x, int y);

// Implement the thread function which will be called by the spawned threads
//
//
// This function is called by the main thread
// It will spawn and join threads, calling a thread function that
// you'll implement
//
void ComputeMandelbrot_multiThreaded(int ** pts, int * starts, int * ends, int dimY, int count) {

	int s,e;
	// Loop through the ranges
	for(int i = 0; i < count; i++)
	{
		s = starts[i];
		e = ends[i];
		
		// Compute MandelbrotPoint (similar to smdb)
		for(int j = s; j < e; j++) {
			for(int k = 0; k < dimY; k++) {
				pts[k][j] = ComputeMandelbrotPoint(j,k);
			}
		}
	}
}



// If the return code is false, disable plotting and reporting which won't
// work if the multithraded code hasn't been implemented
bool Mandelbrot_multiThreaded(int** pts, int dimX, int dimY, int numThreads, int chunkSize) 
{  
      	// Create thread Pool
	thread tpool[numThreads];

	// Set ranges for threads
	int **srange = new int*[numThreads];
	int **erange = new int*[numThreads];	
	
	// locals to help calculate
	int count = 0, itr = 0;
	int x = 0, c;
	
	// Initialize arrays at least at length 1
	for(int i = 0; i < numThreads; i++){
		srange[i] = new int[1];  
		erange[i] = new int[1];
	}	

	// Do Cyclic algorithm
	if(chunkSize) {
		int r;
		c = dimX;
		// Set Start / End Ranges
		while((r = c - (chunkSize * numThreads)) >= 0) {
			for(int i = 0; i < numThreads; i++) 
			{
				srange[i] = (int*) realloc(&srange[i][0], sizeof(int)*(count + 2));
				erange[i] = (int*) realloc(&erange[i][0], sizeof(int)*(count + 2));
				srange[i][count + 1] = 0;
				erange[i][count + 1] = 0;

				srange[i][count] = x;
				x = x + chunkSize;
				erange[i][count] = x;
	
			}
			count++;
			c -=(chunkSize * numThreads);
		}

	}
	// Do Block Partition
	else {
		
		int len = dimX / numThreads;
		c = dimX % numThreads;
		
		// Set Blocks
		for(int i = 0; i < numThreads; i++) 
			erange[i][count] = len;
		
	}

	// Push Leftovers
	while(c) {
		erange[itr++][count] += 1;
		c--;
		if(itr >= numThreads)
			itr = 0;
	}

	// Set last Start/End Ranges
	for(int i = 0; i < numThreads; i++) {
		srange[i][count] = x;
		x+=erange[i][count];
		erange[i][count] = x;
	}
	count++;	
	
	// Start Threads
	for(int i = 0; i < numThreads; i++) {
		tpool[i] = thread(ComputeMandelbrot_multiThreaded
			,pts ,&srange[i][0] , &erange[i][0], dimY, count);
	}
	// Join Threads
	for(int i = 0; i < numThreads; i++) {
		tpool[i].join();
	}	

	// Delete arrays from heap
	for(int i = 0; i < numThreads; i++) {
		delete [] srange[i];
		delete [] erange[i];
	}
	
	delete [] srange;
	delete [] erange;

	// return true
	return true;
}


