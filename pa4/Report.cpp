// 
// Performs various reporting functions
//
// Do not change the code in this file, as doing so
// could cause your submission to be graded incorrectly
//

#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdlib.h>
#include <math.h>

using namespace std;

// Reports statistics about the computation
// These values should not vary (except to within roundoff)
// when we use different numbers of  processes to solve the problem

void ABEND()
{
   cout.flush();
   cerr.flush();
   exit(-1);
}
 
void Stop(){
   cout.flush();
   cerr.flush();
    exit(-1);
}

// Report statistics periodically
void repNorms(double l2norm, double mx,  double dt, int m,int n, int niter, int stats_freq){

        if ( !(niter % stats_freq)) {
            cout <<      setw(6);
            cout.setf(ios::fixed);
            cout << "iteration= " << niter << ", t= ";
            cout.unsetf(ios::fixed);
            cout.setf(ios::scientific);
            cout.precision(6);
            cout << "Max norm: " << mx << ", L2norm: " << l2norm << endl;
        }
}
void printTOD(string mesg)
{
     time_t tim = time(NULL);
     string s = ctime(&tim);

        cout << endl;
        if (mesg.length() ==  0) {
            cout << "Time of day: " << s.substr(0,s.length()-1) << endl;
        }
        else {
            cout << "[" << mesg << "] " ;
            cout << s.substr(0,s.length()-1) << endl;
        }
        cout << endl;
}


// Computes the gigaflops rate

double gflops(int n, int niter, double time){
    int n2 = n*n;
    int64_t updates = (int64_t) n2 * (int64_t) niter;
    int64_t flops = 28 * updates;
    double flop_rate = (double) flops / time;
    return ( flop_rate/1.0e9);
}


void ReportEnd(int niters, double l2norm, double mx, int m,int n, double t0){
        double gf = gflops(n+1, niters, t0);
        printTOD("Simulation ends");
        cout <<          setw(6);
        cout.setf(ios::fixed);
        cout << " iteration " << niters << endl;
        cout.unsetf(ios::fixed);
        cout.setf(ios::scientific);
        cout.precision(6);
        cout << "Max norm: " << mx << ", L2norm: " << l2norm << endl;
        cout.unsetf(ios::scientific);
        cout.unsetf(ios::fixed);
        cout.precision(6);
        cout << "Running Time: " << t0 << " sec." << endl;
        cout.precision(3);
        cout << "GFlop rate: " << gf << endl << endl;

        cout << "    M x N   [#iter]   {T_p, Gflops}   Linf, L2" << endl;
        // This should really print out m and n, but
        // we had to add 1 to be consistent with the reference output
        cout << "@> " << m  << " " << n << " ";
        cout.precision(6);
//        cout <<  "\t[ " << niters << "] ";
        cout <<  "    " << niters << " ";
        cout.precision(4);
//        cout << "{" << t0 << " "  << gf << "} ";
        cout << "  " <<  t0 << " "  << gf;

        cout.unsetf(ios::fixed);
        cout.setf(ios::scientific);
        cout.precision(5);
        cout << "      " << mx << "   " << l2norm << endl;
        cout << "  -----" << endl;
}

void ReportStart(double dt, int niters, int m, int n ){

        printTOD("Simulation begins");
        cout << "dt= " << dt << ", ";
	cout << "# iters = " << niters << endl;
        cout << "m x n = " << m << " x " << n << endl;


#ifdef SSE_VEC
        cout << "Using SSE Intrinsics\n";
#endif

}
