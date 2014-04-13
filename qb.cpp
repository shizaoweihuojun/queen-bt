/*
    N-queen puzzle: backtracking method for find all solutions.
    Main search cycle is parallized.
    Arguments: 1st = board dimension; 2nd = number of threads.
*/

#include <omp.h>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <sstream>

struct data{
    long long num_sln;
    long long ops;
};

bool hit(int x,int y,int * qs,int sz) { 
    for (int i = 0; i < sz; i++)
	    if (y == qs[i] || x-i == abs(y-qs[i])) return true;
    return false;
}

data solver(int p,int sz, bool debug = false){
    int * qs = new int [sz]; 
    qs[0] = p;
    int nq = 1;
    int j0 = 0;
    data d;
    d.num_sln = d.ops = 0;
    
    while (nq > 0) { 
	    for (int i = nq; i < sz; i++) {
	        for (int j = j0; j < sz; j++) { 
    		    d.ops++;
	        	if (!hit(i,j,qs,nq)) {
		            qs[nq++] = j;
		            break;
		        }
	        }
	        if (i+1 > nq) break; 
	        j0 = 0;
	    }
	
	    // solution is found
    	if (nq == sz) {
	        d.num_sln++;
	        if (debug){
		        #pragma omp critical
		        {
		            std::cout << omp_get_thread_num() << " thread: " << 
		             d.ops << " op; sol N" << d.num_sln << ": ";
		            for (int i = 0; i < sz; i++) std::cout << qs[i];
		            std::cout << std::endl;
		        }
	        }
	    }
	
	    // remove queens till may put a new one
	    j0 = sz;
	    while (j0 == sz) j0 = qs[--nq] + 1;
    }
    delete[] qs;
    return d;
}

int main(int argc, char* argv[]) {
    long sz = (argc > 1) ? atol(argv[1]) : 8;
    int num_threads = (argc > 2) ? atoi(argv[2]) : 1; 
    
    long long total_sln = 0;
    long long total_ops = 0;
    omp_set_num_threads(num_threads);
    
    double tm = omp_get_wtime();
    #pragma omp parallel for shared(sz,total_sln,total_ops)\
     schedule(runtime)
    for (int p = 0; p < sz; p++) {
	    data d = solver(p,sz);
	    #pragma omp atomic
	    total_sln += d.num_sln;
	    #pragma omp atomic
	    total_ops += d.ops;
    }
    tm = omp_get_wtime() - tm;
    		
    std::cout << sz << "-board: "<< num_threads << " threads. Number of solutions: " << 
     total_sln << ". Combinations checked: " << total_ops << ". Time left: " << 
     std::fixed << tm << "s." << std::endl;
    return 0;
}

