#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<omp.h>
#include <vector>
#include <list>
#include<stack>
#include<queue>
#include"graph.hpp"

// Limits for array sizes
#define n_len 1000005
#define e_len 8000005

using namespace std;

// reset queue
void clear( std::queue<long long int> &q ) {
   std::queue<long long int> empty;
   std::swap( q, empty );
}
// reset stack
void clear_stack( std::stack<long long int> &s ) {
   std::stack<long long int> empty;
   std::swap( s, empty );
}

// variables
long long int V[n_len], E[e_len], dist[n_len];
float cb[n_len], delta[n_len], sigma[n_len];

// MAIN
int main() {

    // ================================ READ INPUT AND MAKE Compressed Adjancency List ====================================
     graph G("smallData.txt");
     G.parseGraph();
    vector<vector<long long int> > p(G.num_nodes());
       int countval=0; 
	
    // BC value stored here
    memset(cb,0,sizeof(G.num_nodes()));

    // max bc var
    float max_bc = 0.0;
    // ===================================================== MAIN CODE =================================================== 
    
    
   // int seeds[5]={267649,518682,10978,22382,43958};
      double startTime=omp_get_wtime();
	for(long long int ii = 0; ii < G.num_nodes(); ++ii) {
        // ============== INIT ========================
        int i=ii;
        stack<long long int> s;
        for(long long int j = 0; j < G.num_nodes(); ++j){
            p[j].clear();
        }
	
        for(long long int j=0;j<G.num_nodes();++j) dist[j]=-1,sigma[j]=0.0,delta[j]=0.0;
		sigma[i]=(float)1;
		dist[i]=0;
		// queue<long long int> q;
        // q.clear();
		// clear(q);
        queue<long long int> q;
        // ============== distance and sigma calculations ====================
        q.push(i);
		while(!q.empty()){
			long long int cons=q.front();
			q.pop();
			s.push(cons);
			for(long long int j = G.indexofNodes[cons]; j < G.indexofNodes[cons+1]; ++j){
                int nbr=G.edgeList[j];
				if(dist[nbr]<0){
					dist[nbr]=dist[cons]+1;		
                    q.push(nbr);
				}	
                if(dist[nbr]==dist[cons]+1){
                   
                    sigma[nbr]+=sigma[cons];
                    p[nbr].push_back(cons);
               
                }
			}		
		}
        
       /* for(int i=0;i<G.num_nodes();i++)
           printf("%d %f\n",i,sigma[i]);*/
        
        // ============== BC calculation ========================
     
       

        while(!s.empty()){
            long long int cons=s.top();
            s.pop();
            long long int upto = p[cons].size();
            
            // ============== Using parent's sigma ========================
                
            for(long long int j = 0; j < upto;++j){
                if(p[cons][j]==4&&ii==4)
                   countval++;
                  
                delta[p[cons][j]]+=(sigma[p[cons][j]]/sigma[cons])*((1+delta[cons]));
            }
                  
                    cb[cons]+=delta[cons];
                    max_bc = (max_bc > cb[cons])?max_bc:cb[cons];
                                      
            
        }

         
	}
  
    double endTime=omp_get_wtime();
    printf("EXECUTION TIME %f \n",endTime-startTime);
  

    // ===================================================== RESULTS ===================================================

     char *outputfilename = "outputC.txt";
	FILE *outputfilepointer;
	outputfilepointer = fopen(outputfilename, "w");

    	for (int i = 0; i <=G.num_nodes(); i++) {
		  fprintf(outputfilepointer, "%d  %f\n",i,cb[i]);
	}

    cout<<"\n";

  
    
    // ================================ Max BC value ====================================
    // Max BC value
    cout<<"Max BC value: "<<max_bc/2.0<<endl;

	return 0;
}