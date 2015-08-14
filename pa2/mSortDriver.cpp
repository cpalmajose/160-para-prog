// Driver for Merge sort program
// Scott B. Baden, CSE Dept, UCSD
// 9/28/13
// 

#include <cstdlib>
#include <iostream>     // std::cout

#include <algorithm>    // std::sort
#include <vector>       // std:vector

#include <assert.h>

#include <thread>

#include <time.h>
#include "cblock.h"

//
// Globals
//
  control_block cb;

int cmdLine(int argc, char **argv);
bool verify(std::vector<int> keys, int lo, int hi);


template < class List >
inline void Swap( List& A, List& B )
{
    List t = A;
    A = B;
    B = t;
}

double getTime();

void DC_MergeSort(std::vector<int> *keys0, int l0, int l1, std::vector<int> *keys1, int level, bool& new_dir);

void printVec(std::vector<int> myvector){
    for (std::vector<int>::iterator it=myvector.begin(); it!=myvector.end(); ++it)
        std::cout << ' ' << *it;
    std::cout << std::endl;
}
void printVec(std::vector<int> myvector, int n){
    for (std::vector<int>::iterator it=myvector.begin(); it!=myvector.begin()+n; ++it)
        std::cout << ' ' << *it;
    std::cout << std::endl;
}



int main(int argc, char **argv){
// Parse commmand line arguments
// Default values
   cmdLine(argc, argv);
   int N = cb.N;
   int NT = cb.NT;
 
 // Report on the input to the program 
   std::cout << "# points: " << N << std::endl;
   std::cout << "# threads: " << NT << std::endl;
   std::cout << "# Reps: " << cb.nreps << std::endl;
   std::cout << "Minimum size for mergeSort: " << cb.minN << std::endl;
   if (cb.par_merge)
        std::cout << "Using PARALLEL merge\n";
   else
        std::cout << "Using SERIAL merge\n";
        
   if (cb.worst)
        std::cout << "Worst case (reverse sorted) input\n";
   if (cb.best)
        std::cout << "Best case (perfectly sorted) input\n";
    std::vector<int> *keys0 = new std::vector<int>();
    assert(keys0);
 // Initialize input keys
    if (cb.worst){
        for (auto i = 0; i < N; i++) 
             keys0->push_back(N-i-1);
    }
    else if (cb.best){
        for (auto i = 0; i < N; i++)
             keys0->push_back(i);
    }
    else{ // Random input
 
 
     // Initialize (seed) the random number generator
        if (cb.seedVal == 0){
            cb.sd = time(NULL);
            srand(cb.sd);
            std::cout << "Random # generator seed: " << cb.sd << std::endl;
        }
        else{
            cb.sd = cb.seedVal;
            srand(cb.seedVal);
            std::cout << "Random # generator seed: " << cb.seedVal << std::endl;
        }
     
        for (auto i=0; i<N; i++)
            keys0->push_back(rand());
   }

   std::vector<int> *keys1 = new std::vector<int>(keys0->size(),0);
   assert(keys1);
   int n = keys0->size();
 
   if (n <= 8){    // Print the keys0 for short vectors, change threshold
                   // if necessary 
       std::cout << "Input keys:\n";
       printVec(*keys0);
   }

   double tp = -getTime();

   // Start off with the data stored in buffer 0
   int level = 0;
   bool new_dir;
   DC_MergeSort(keys0, 0, N-1, keys1, level, new_dir);

   // Swap the keys if the final merge was sent to the 2nd buffer
   if (!new_dir){
       Swap(keys0,keys1);
    }

   tp += getTime();

   bool pass = verify(*keys0, 0, N-1);
   if (pass)
      std::cout << "Sorted result PASSED verification test.\n\n";
   else{
      std::cout << "*** Result has FAILED the verification test!\n\n";
      if (n <= 16){    // Print the keys for short vectors, change the cutoff at will
          std::cout << "Sorted Keys:\n";
          printVec(*keys0);
      }
    }

   char PMERGE = cb.par_merge ?  'Y': 'N';
   char PF = pass ?  'P': 'F';
   char IN = cb.best ? 'B' : (cb.worst ? 'W' : 'R');

   std::cout << "Ran on " << NT << " threads for " << cb.nreps << " reps\n";
   std::cout << "Wall clock running time: " << tp << " sec." << std::endl;
   //    This is a shorthand form of all diagnostic output, to assist
   //    in analyzing large amounts of output
   //    Grep on the pattern '@>' and only the statistics will be returned


   // Could Change to c++ style formatting
   printf("\n        N     NT    T_P            G      Reps In  ||?  V \n");
   printf("@> %9d %3d   %7.3f   %9d %6d   %c   %c   %c\n\n",N,NT, tp, cb.minN, cb.nreps, IN, PMERGE, PF);


  if (n <= 8){    // Print the keys for short vectors, change the cutoff at will
      std::cout << "Sorted Keys:\n";
      printVec(*keys0);
  }

  delete keys0;
  delete keys1;
  return 0;
}

