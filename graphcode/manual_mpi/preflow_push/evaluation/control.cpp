#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>

using namespace std;

/*
    Implementation of highest-label push-relabel maximum flow
    with gap relabeling heuristic.

    Running time:
        O(|V|^2|E|^{1/2})

    Usage:
        - add edges by AddEdge()
        - GetMaxFlow(s, t) returns the maximum flow from s to t
    
    Input:
        - graph, constructed using AddEdge()
        - (s, t), (source, sink)

    Output:
        - maximum flow value

    Todo:
        - implement Phase II (flow network from preflow network)
        - implement GetMinCut()
*/

template <class T> struct Edge {
    int from, to, index;
    T cap, flow;

    Edge(int from, int to, T cap, T flow, int index): from(from), to(to), cap(cap), flow(flow), index(index) {}
};

template <class T> struct PushRelabel {
    int n;
    vector <vector <Edge <T>>> adj;
    vector <T> excess;
    vector <int> dist, count;
    vector <bool> active;
    vector <vector <int>> B;
    int b;
    queue <int> Q;

    PushRelabel (int n): n(n), adj(n) {}

    void AddEdge (int from, int to, int cap) {
        adj[from].push_back(Edge <T>(from, to, cap, 0, adj[to].size()));
        if (from == to) {
            adj[from].back().index++;
        }
        adj[to].push_back(Edge <T>(to, from, 0, 0, adj[from].size() - 1));

    }

    void Enqueue (int v) {
        if (!active[v] && excess[v] > 0 && dist[v] < n) {
            active[v] = true;
            B[dist[v]].push_back(v);
            b = max(b, dist[v]);
        }
    }

    void Push (Edge <T> &e) {
        T amt = min(excess[e.from], e.cap - e.flow);
        if (dist[e.from] == dist[e.to] + 1 && amt > T(0)) {
            e.flow += amt;
            adj[e.to][e.index].flow -= amt;
            excess[e.to] += amt;    
            excess[e.from] -= amt;
            Enqueue(e.to);
        }
    }

    void Gap (int k) {
        for (int v = 0; v < n; v++) if (dist[v] >= k) {
            count[dist[v]]--;
            dist[v] = max(dist[v], n);
           count[dist[v]]++;
            Enqueue(v);
        }
    }

    void Relabel (int v) {
        count[dist[v]]--;
        dist[v] = n;
        for (auto e: adj[v]) if (e.cap - e.flow > 0) {
            dist[v] = min(dist[v], dist[e.to] + 1);
        }
        count[dist[v]]++;
        Enqueue(v);
    }

    void Discharge(int v) {
        for (auto &e: adj[v]) {
            if (excess[v] > 0) {
                Push(e);
            } else {
                break;
            }
        }

        if (excess[v] > 0) {
            if (count[dist[v]] == 1) {
                Gap(dist[v]); 
            } else {
                Relabel(v);
            }
        }
    }

    T GetMaxFlow (int s, int t) {
        dist = vector <int>(n, 0), excess = vector<T>(n, 0), count = vector <int>(n + 1, 0), active = vector <bool>(n, false), B = vector <vector <int>>(n), b = 0;
        
        for (auto &e: adj[s]) {
            excess[s] += e.cap;
        }

        count[0] = n;
        Enqueue(s);
        active[t] = true;
        
        while (b >= 0) {
            if (!B[b].empty()) {
                int v = B[b].back();
                B[b].pop_back();
                active[v] = false;
                Discharge(v);
            } else {
                b--;
            }
        }
        return excess[t];
    }

    T GetMinCut (int s, int t, vector <int> &cut);
};


int main(int argc, char* argv[]) {
    // Check if the filename is provided as a command-line argument
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <filename>" << endl;
        return 1;
    }

    // Extract the filename from the command-line arguments
    string fileName = argv[1];

    // Open the file
    ifstream inputFile(fileName);
    if (!inputFile.is_open()) {
        cerr << "Error: Unable to open file " << fileName << endl;
        return 1;
    }

    // Read edges from the file and find the maximum node index to determine the number of nodes
    int u, v, w, maxNode = 0;
    while (inputFile >> u >> v >> w) {
        maxNode = max(maxNode, max(u, v));
    }

    // Close the file and reopen to read from the beginning
    inputFile.close();
    inputFile.open(fileName);

    // Number of nodes is the maximum node index + 1
    int numNodes = maxNode + 1;

    // Create an instance of PushRelabel with the specified number of nodes
    PushRelabel<int> pr(numNodes);

    // Read edges from the file and add them to the flow network
    while (inputFile >> u >> v >> w) {
        pr.AddEdge(u, v, w);
    }

    // Close the file
    inputFile.close();

    // Find the maximum flow from node 0 to the last node (assuming the last node is the sink)
    int source = 0;
    int sink = 1;
    int maxFlow = pr.GetMaxFlow(source, sink);

    cout << maxFlow << endl;

    return 0;
}

 
