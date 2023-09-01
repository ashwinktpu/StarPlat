#include <iostream>
#include <iomanip>
#include <stack>
#include <queue>
#include "../graph.hpp"

#include <thread>
#include <chrono>

#define NL '\n'
#define debug_flag false
#define DEBUG if(debug_flag) 

using namespace std;

void PrintStack(stack<int>& s) {

	std::vector<int> tempVec;
	
    while(!s.empty()) {
        tempVec.push_back(s.top());
        s.pop();
    }

	for(auto i = tempVec.rbegin(); i != tempVec.rend(); ++i) {
        std::cout << *i << " ";
    }

	for(auto i = tempVec.rbegin(); i != tempVec.rend(); ++i) {
        s.push(*i);
    }

	cout << NL;
}

void ComputeBetweenessCentrality(graph& g, vector<int>& src) {

	unsigned V = g.num_nodes();
	unsigned E = g.num_edges();


	printf("Nodes : %d\n", V);
	printf("Edges : %d\n", E);

	int *h_meta = (int*) malloc(V * sizeof(int));
	int *h_data = (int*) malloc(E * sizeof(int));

	for(int i=0; i<= V; i++)
		h_meta[i] = g.indexofNodes[i];

	for(int i=0; i< E; i++)
		h_data[i] = g.edgeList[i];

	vector<double> bc(V, 0);

	for(int s : src) {

		stack<int> S;
		vector<vector<int>> path(V);
		vector<double> sigma(V, 0);
		sigma[s] = 1;
		vector<double> d(V, -1);
		d[s] = 0;
		queue<int> Q;

		DEBUG cout << "Current src: " << s << NL;

		Q.push(s);

		while(!Q.empty()) {

			int v = Q.front();
			Q.pop();
			S.push(v);

			for (int edge = h_meta[v]; edge < h_meta[v+1]; edge++) {  

				int w = h_data[edge];

				if(d[w] < 0) {

					Q.push(w);
					d[w] = d[v] + 1;
				}

				if(d[w] == d[v] + 1) {

					sigma[w] += sigma[v];
					path[w].push_back(v);
				}
			}
		}

		vector<double> delta(V, 0);

		while(!S.empty()) {

			int w = S.top();
			S.pop();

			for(int v : path[w]) 
				delta[v] += (sigma[v] / sigma[w]) * (1 + delta[w]);

			// DEBUG
			// cout << "DELTA ";
			// for(int i = 0; i < 5; i++)
			// 	cout << delta[i] << " ";
			// 	cout << NL << "BC    ";

			if(w != s)
				bc[w] += delta[w];

			// DEBUG
			// for(int i = 0; i < 5; i++)
			// 	cout << bc[i] << " ";
			// 	cout << NL;
		}
	}

	for(int i : src)
		cout << i << " " << bc[i] << NL;
}

int main(int argc, char ** argv) {

	graph G(argv[1]);
	G.parseGraph();

	vector<int> src = {1,2,3};

	ComputeBetweenessCentrality(G, src);

	// std::this_thread::sleep_for(std::chrono::milliseconds(10000));
	// cout << "flag;";

	return 0;
}
