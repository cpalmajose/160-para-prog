// Merge Sort Starter code
// Scott B. Baden, CSE Dept, UCSD
// 10/2/2013
// Modified 4/15/2015
// 

#include <cstdlib>
#include <iostream>     // std::cout
#include <assert.h>

#include <algorithm>    // std::sort
#include <vector>       // std:vector

#include <thread>
#include <time.h>
#include "cblock.h"

//
// Globals
//
  extern control_block cb;

inline
int ilog(int x){
    assert(x > 0);
    for (int z=0; z< 33; z++){
        if (x == 1)
            return(z);
        x >>= 1;
    }
    assert(1);          // This should never happen, failure if we get here
    return(-77);        // Return a value to humor the compiler
}

double getTime();

// Do the local sort
void LSort(std::vector<int> *keys, int l0, int l1, int level){

    std::vector<int>::iterator I0 = keys->begin()+l0;
    std::vector<int>::iterator I1 = keys->begin()+l1+1;
    if ((l1-l0+1) <= 16){
        std::cout << "Keys to be sorted: \n";
        for (std::vector<int>::iterator it=I0; it!=I1; ++it)
            std::cout << ' ' << *it;
        std::cout << std::endl;
        std::cout << std::endl;
    }

    std::stable_sort(I0,I1);

    return;
}

//
// Merge two lists on the interval (l0:l1), that is,
// (l0:mid-1) and (mid-1:n1), where mid = l0 + n/2
// Serial
// 

void Merge(std::vector<int> *keysIn, std::vector<int> *keysOut,
           int l0, int l1){
    int n = (l1-l0) + 1;
    int mid = l0 + n/2;
    int min1 = l0, max1 = mid-1;
    int min2 = mid, max2 = l1;

#ifdef DEBUG
// Output the keys if N is <= 16
    if (n <= 16){
        for (int i=min1;i<=max1;i++) 
            std::cout<<(*keysIn)[i]<<" ";
        std::cout<<std::endl;
        for (int i=min2;i<=max2;i++) 
            std::cout<<(*keysIn)[i]<<" ";
        std::cout<<std::endl;
    }
#endif
    int l=min1;
    int r=min2;
    int i;
    for (i=0; i < max2-min1+1 ; i++) {  
      if ((*keysIn)[l]<(*keysIn)[r]) {
          (*keysOut)[i+min1]=(*keysIn)[l++];
          if (l>max1) break;
      } else {
          (*keysOut)[i+min1]=(*keysIn)[r++];
          if (r>max2) break;
      }
    }
    while (l<=max1) {
        i++;
        (*keysOut)[i+min1]=(*keysIn)[l++];
    }
    while (r<=max2) {
        i++;
        (*keysOut)[i+min1]=(*keysIn)[r++];
    }
}

// Binary Search
int BinarySearch(int value, std::vector<int> *in, int s_index, int size) {

	// Get ending Index
	int e_index = s_index + size;
	
	// Initialize Iterators
	std::vector<int>::iterator b=(*in).begin(), e=(*in).begin(), low;
	b+= s_index;
	e+= e_index;
	low = std::upper_bound(b,e,value);
	
	// Return Index
	return (low - b);

}

// Parallel Merge Algorithm
// From Slides with slight Modification
void P_Merge(std::vector<int> *in, std::vector<int> *out, int w_index, int a_start,
	int b_start, int a_size, int b_size, int level)
{
	if(a_size < b_size)
		P_Merge(in, out, w_index, b_start, a_start, b_size, a_size, level);
	else if(a_size == 1) {
		if(b_size == 0)
			(*out)[w_index] = (*in)[a_start];
		else {
			(*out)[w_index] = ((*in)[a_start] < (*in)[b_start]) ?
				(*in)[a_start] : (*in)[b_start];
			(*out)[w_index+1] = ((*in)[a_start] < (*in)[b_start]) ?
				(*in)[b_start] : (*in)[a_start];

		}
	}
	else if(b_size == 0) {
		for(int i = 0; i < a_size; i++) {
			(*out)[w_index + i] = (*in)[a_start + i];
		}
	}
	else {
		int m2 = a_size / 2;
		int j = BinarySearch((*in)[a_start + m2], in, b_start, b_size);
		int maxLevel = ilog(cb.NT); 
		
		// Spawn Threads if Able
		if(level < maxLevel){
			std::thread left(P_Merge, in, out, w_index, a_start, b_start
				, m2, j, level + 1);
			std::thread right(P_Merge, in, out, w_index+m2+j, a_start+m2
				, b_start+j, a_size-m2, b_size-j, level + 1);

			left.join();
			right.join();	
		}
		// Serial Merge
		else {
			int a_ctr = 0, b_ctr = 0;
			int a_itr = a_start, b_itr = b_start;
			while(a_ctr < a_size && b_ctr < b_size)
			{
				if((*in)[a_itr] < (*in)[b_itr]) {
					(*out)[w_index++] = (*in)[a_itr++];
					a_ctr++;
				}
				else {
					(*out)[w_index++] = (*in)[b_itr++];
					b_ctr++;
				}
			}
			while(a_ctr < a_size){
				(*out)[w_index++] = (*in)[a_itr++];
				a_ctr++;
			}
			while(b_ctr < b_size) {
				(*out)[w_index++] = (*in)[b_itr++];
				b_ctr++;
			} 
		}
	}

}



//
// Divide and conquer algorithm for Merge Sort
//

// The boolean flag
void DC_MergeSort(std::vector<int> *keysIn, int l0, int l1, std::vector<int> *keysOut, int level, bool& direction_flag)
{

    int n = (l1-l0) + 1;
    if (n < 2){               // If list has 0 or 1 elements,
        direction_flag = true;
        return;       // then it is already sorted
    }

    // Otherwise, split list at the mid-point, and sort each part recursively
    // Upon return, we merge the two lists
    // We stop the recursion at a specified threshold value minN
    // and at that point we use a "base" sort
    if (n <= cb.minN){
        LSort(keysIn,l0,l1,level);
        direction_flag = true;
    }
    else{
        int maxLevel = ilog(cb.NT);     // Take log of max threads (taken from input parameter)
        int mid = l0+n/2;
        bool dir0, dir1;
        int levNext0 =  level+1;
        int levNext1 =  level+1;
        assert(levNext0 == levNext1);   // Sanity Check
                                        // Error if they aren't equal!

        if( level >= maxLevel ) { 
            // Cannot spawn anymore threads, so do recursive call (no thread spawning)
            DC_MergeSort(keysIn, l0, mid-1, keysOut, levNext0,dir0);
            DC_MergeSort(keysIn, mid, l1, keysOut, levNext1,dir1);
        } else {
            // Spawn threads then divide and conquer
            std::thread thread1(DC_MergeSort, std::ref(keysIn), l0, mid-1, std::ref(keysOut), levNext0, std::ref(dir0));
            std::thread thread2(DC_MergeSort, std::ref(keysIn), mid,   l1, std::ref(keysOut), levNext1, std::ref(dir1));

            thread1.join();
            thread2.join();
        }

        assert(dir0 == dir1);           // Sanity check
                                        // Error if they aren't equal!

// Do the merge with a serial algorithm
// As we merge, we place the merged output in a different list from
// that containing the input to the merge, and we alternate
// as we merge back up to the root of the divide and conquer call tree
//
// Since we sort in place for an input size of 2, or
// when the recursion terminates (and we use qsort to sort in place),
// we set the flag to true in these cases
//
// Can you avoid the need for a 2nd buffer?
//
	        
	// Determines Serial of Parallel Merge
        if (dir0) {
		if(cb.par_merge) {
			P_Merge(keysIn, keysOut, l0, l0, mid, mid-l0, l1-(mid-1), level+1);
		}
		else {
			Merge(keysIn,keysOut,l0,l1);
		}
	
	}
        else {
		if(cb.par_merge) {
			P_Merge(keysOut, keysIn, l0, l0, mid, mid-l0, l1-(mid-1), level+1);
		}
		else {
            		Merge(keysOut,keysIn,l0,l1);
		}
	}

        direction_flag = !dir0;
        return;
    }

}
