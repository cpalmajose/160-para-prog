// Process command line arguments
// 
//
// Do not change the code in this file, as doing so
// could cause your submission to be graded incorrectly
//
#include <assert.h>
#include <getopt.h>
#include <stdlib.h>
#include <iostream>
#include "omp.h"

using namespace std;

void Stop();

void cmdLine(int argc, char *argv[], int& n, int& stats_freq, int& plot_freq, int &niter, bool &debug){
/// Command line arguments
 // Default value of the domain sizes
 static struct option long_options[] = {
        {"n", required_argument, 0, 'n'},
	{"niter", required_argument, 0, 'i'},
        {"nt", required_argument, 0, 't'},
        {"stats-freq", required_argument, 0, 's'},
        {"plot", required_argument, 0, 'p'},
        {"debug", no_argument, 0, 'd'},

 };
    // Process command line arguments
 for(int ac=1;ac<argc;ac++) {
    int c;
    while ((c=getopt_long(argc,argv,"n:i:t:s:p:d",long_options,NULL)) != -1){
        switch (c) {

	    // Size of the computational box
            case 'n':
                n = atoi(optarg);
                break;

            // # of iterations
	    // Use this option control the number of mesh sweeps
            case 'i':
                niter = atoi(optarg);
                break;

	    // Print statistics for assessing correctness
            case 's':
                stats_freq = atoi(optarg);
                break;

	    // Plot the excitation variable
            case 'p':
                plot_freq = atoi(optarg);
                break;
                //
	    // Plot the excitation variable
            case 'd':
                debug = true;
                break;

	    // Error
            default:
                cout << endl;
                cout << "Usage: apf [-n <domain size>]     [-i <# iterations>]        [-d]";
                cout << "\n\t   ";
                cout << "[-s <stats frequency>] [-p <plot frequency>]\n\t";
                cout << endl;
                exit(-1);
            }
    }
 }

}

