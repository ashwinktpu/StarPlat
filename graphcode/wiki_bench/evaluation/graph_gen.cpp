#include <iostream>


void fileIO(){
    freopen("input.txt", "r", stdin);
    freopen("input.txt", "w", stdout);
}

int main (int argc, char ** argv) {

    // fileIO () ;
    srand (atoi(argv[1])) ;
    int v = 1000;
    int e = 8000;
    for (int i=0; i<e; i++) {
        int u, x, w ;
        u = rand () % v ;
        x = rand () % v ;
        w = rand () % v ;
        if (u==x) {i-- ; continue;}
        printf ("%d %d %d\n", u, x, w ) ;
    }
    int temp = rand () % v ;
    while (temp == 0 ) {
        temp = rand () % v ;
    }
    int twe = rand () % v ;
    printf ("0 %d %d\n", temp, twe) ;
    temp = rand () % v ;
    while (temp == 1) {
        temp = rand () % v ;
    }
    twe = rand () % v ;
    printf ("%d 1 %d\n", temp, twe) ;
}
