#include <iostream>
#include <unordered_map>
#include <utility>
#include <set>


void fileIO(){
    freopen("input.txt", "r", stdin);
    freopen("input.txt", "w", stdout);
}

int main (int argc, char ** argv) {

    fileIO () ;
    srand (atoi(argv[1])) ;
    int numVertices = 7 ;
    int sparsityFactor = 5 ;
    std::set<std::pair<int,int> > recorder ;
    int source = 0, sink = 1 ;
    int fromSource = rand () % numVertices ;
    while (fromSource == 0) {
        fromSource = rand () % numVertices ;
    } 
    printf ("%d %d %d\n", source, fromSource, rand () % 100) ;
    recorder.insert ({source, fromSource}) ;
    recorder.insert ({fromSource, source}) ;
    int fromSink = rand () % numVertices ;
    while (fromSink == sink) {
        fromSink = rand () % numVertices ;
    } 
    printf ("%d %d %d\n", fromSink, sink, rand () % 100) ;
    recorder.insert ({sink, fromSink}) ;
    recorder.insert ({fromSink, sink}) ;
    for (int u = 0; u < numVertices; u++) {
        for (int v = 0; v < numVertices; v++) {
            if (u != v) {
                if (recorder.find ({u,v}) == recorder.end () && rand () % sparsityFactor == 0) {
                    printf ("%d %d %d\n", u, v, rand () % 100);
                    recorder.insert ({u,v}) ;
                    recorder.insert ({v,u}) ;
                }
            }
        }
    }
}

