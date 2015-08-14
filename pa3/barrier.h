#ifndef _BARRIER_H
#define _BARRIER_H

#include <mutex>
#include <cassert>
#include <condition_variable>
#include <vector>
#include <atomic>

//#include <stdlib.h>
//#include <stdio.h>
//#include <iostream>

using namespace std;

#ifdef LOG_BARRIER
class barrier
{

private:
    
    atomic<bool> * TIDs;
    atomic<int> m_ndone;
    atomic<bool> notpass;
    atomic_flag lock_vector;
    int m_nt;

public:
    // Fill in your definition of the barrier constructor
    barrier(int NT = 2) : m_ndone(0), notpass(true)
	,lock_vector(ATOMIC_FLAG_INIT), m_nt(NT){ 

	TIDs = new atomic<bool>[NT];
	for(int i = 0; i < m_nt; i++) {
	        TIDs[i].store(false, memory_order_release);
	}	
   }

   ~barrier() {
	delete [] TIDs;
    }

    // Fill in your definition of the barrier synchronization function
    void bsync(int TID) {
	
	int pos,lchild,rchild;

	// Only pass when parent is done
	while(!notpass.load(memory_order_acquire)) { }	

	// lock vector using spin lock
	while(lock_vector.test_and_set()) { }
	
	// add to vector
	pos = m_ndone.load(memory_order_acquire);	
	m_ndone++;	
	TIDs[pos].store(true, memory_order_release);

	// release vector lock
	lock_vector.clear();
	
	//
        lchild = (2 * pos) + 1;
        rchild = (2 * pos) + 2;	
		
	// if parent
	if(pos == 0) {
	    
	    // spin lock till all at barrier
	    while(m_ndone.load(memory_order_acquire) < m_nt) { }
	    notpass.store(false, memory_order_release);

	    while(lock_vector.test_and_set()) {}
	    
	    m_ndone--;	    
	
	    lock_vector.clear();
	    //lchild = (2 * pos) + 1;
 	    //rchild = (2 * pos) + 2;

	    TIDs[pos].store(false, memory_order_release);
	    if(lchild < m_nt) 
		TIDs[lchild].store(false, memory_order_release);
	    if (rchild < m_nt) 	
		TIDs[rchild].store(false, memory_order_release);

	}
	else {
	    	   
	    // spin until all previous threads released 
	    while(notpass.load(memory_order_acquire)) { }
	    while(TIDs[pos].load(memory_order_acquire)) { }
	  
 	    // Release children
	    	    
	    //while(lock_vector.test_and_set()) {}

	    if(lchild < m_nt) 
		TIDs[lchild].store(false,memory_order_release);
	    if (rchild < m_nt) 	
		TIDs[rchild].store(false,memory_order_release);
	    
	    while(lock_vector.test_and_set()) {}
	    m_ndone--;
	    lock_vector.clear();

	    if(m_ndone.load(memory_order_relaxed) == 0)
		notpass.store(true, memory_order_release);

	    //lock_vector.clear();
	}
    }
};
#else
class barrier
{
public:
  explicit barrier(int a_numThreads):m_nt(a_numThreads), m_ndone(0){ };
  inline void bsync(int dummy);
  
private:
  barrier(const barrier&);  // disable copying barriers.
  barrier(); //no null constructor.
  barrier& operator=(const barrier&); // disable assignment
  int m_nt;
  int m_ndone;
  std::mutex m_mutex;
  std::condition_variable m_cv;
};

inline void barrier::bsync(int dummy)
{
  std::unique_lock<std::mutex> lk(m_mutex,std::defer_lock);
  lk.lock();
  if(++m_ndone < m_nt)
     m_cv.wait(lk);
  else {
      m_ndone = 0;
      m_cv.notify_all();
      return;
    }
}


#endif
#endif

