#include <cstdlib>
#include <iostream>
#include <algorithm>    // std::binary_search
#include <vector>
#include "cblock.h"

//
//
//  Sweep over the data verifying that it is correct
//  Note that verification of worst and best case input is trival
//  as the keys are taken from a simple, known, arithmetic progression.
//
//  Random input is different. This test is weak in the sense that,
//  in confirm only that the Keys list is sorted, not that the Keys are correct!
//
//  For that you need to also check that every input key is found
//  in the list of output keys, and you may enable this code
//  by uncommenting the line shown below
//  Beware, as this test is costly
//
//

extern  control_block cb;

bool verify(std::vector<int> Keys, int lo, int hi) {
    for (auto i=lo;i<hi; i++) {
        if (Keys[i] > Keys[i+1]){
            return(0);
	}
    }

    if (cb.worst){
        for (auto i=lo;i<hi; i++) {
             if (Keys[i] != i)
                 return(0);
        }
    }
    else if (cb.best){
        for (auto i=lo;i<hi; i++) {
             if (Keys[i] != i)
                 return(0);
        }
    }
    else{ // Random input exact test
  
#ifdef EXACT_VERIFY
// This is very costly: we search the sorted
// list for each generated key
 
     // Initialize (seed) the random number generator
        srand(cb.sd);
     
        std::vector<int>::iterator I0 = Keys.begin();
        std::vector<int>::iterator I1 = Keys.begin() + cb.N;
        for (auto it=I0; it != I1; it++){
             int r = rand();
            if (!std::binary_search(I0, I1, r))
                return 0;
        }
#endif
   }
   return 1;
}
