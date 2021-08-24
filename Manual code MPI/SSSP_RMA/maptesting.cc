#include <iostream>
#include <boost/mpi.hpp>
#include <vector>
#include<map>
#include <bits/stdc++.h>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/mpi/collectives.hpp>
using namespace std;
namespace mpi = boost::mpi;
int main(int argc, char* argv[])
{
	mpi::environment env(argc, argv);
  	mpi::communicator world;

	vector<map<int,int>> v(2);
     vector<map<int,int>> ov(2);
	std::map<int,int>::iterator itr;
	cout<<"World size "<<world.size()<<endl;
            /*
	// Print the vector of map#include /boost/serialization/map.hpp
   	 for (int i = 0; i < v.size(); i++) {
 
        cout << "Frequency upto "
             << "position " << i + 1
             << endl;
 
        // Traverse the map
        for (auto x : v[i])
            cout << x.first << "-" << x.second << endl;
    }*/
    if (world.rank() == 0) 
    {
    	cout<<"size of vector is "<<v.size()<<endl;
	v[0][1] = 7;
	//cout<<"Echeck "<<v[0][1]<<endl;
	v[1][6] = 8;
	v[0][5] = 10;
     v[1][3] = 5;
     v[1][4] = 6;
	itr = v[1].find(1);
     	 if (itr != v[1].end())
            itr->second = min(v[1][1],4);
    	//world.send( 1, 0, v );
         all_to_all(world,v,ov);
    }
    else
    {
         v[0][2] = 19;
	//cout<<"Echeck "<<v[0][1]<<endl;
	v[1][3] = 18;
	v[0][7] = 10;
     v[1][8] = 15;
	itr = v[0].find(1);
     	 if (itr != v[0].end())
            itr->second = min(v[0][1],9);
    	//world.recv( 0, 0, v );
         all_to_all(world,v,ov);
    }
	// Print the vector of map#include /boost/serialization/map.hpp
   	 for (int i = 0; i < ov.size(); i++) {
 
        cout << world.rank() <<"Frequency upto "
             << "position " << i + 1
             << endl;
 
        // Traverse the map
        for (auto x : ov[i])
            cout << x.first << "-" << x.second << endl;
            }
	return 0;
}
