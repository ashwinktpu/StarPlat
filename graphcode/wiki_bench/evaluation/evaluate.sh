#g++ -std=c++17 main.cpp -o personal

g++ -std=c++17 graph_gen.cpp -o generator
g++ -std=c++17 control.cpp -o control 
g++ -std=c++17 splitter.cpp -o splitter
cd ../distributedv2.0
mpic++ -std=c++17 -O3 -O2 -Ofast -o testing_distributed.out init_maxflow.cpp 
cd ../evaluation

for ((i=1;i<=1000;i++)) ; do

    ./generator $i >input.txt

    ./splitter 

    ./control input.txt > out2

    cd ../distributedv2.0
    bash runner.sh
    wait

    cd ../evaluation
    echo $i
    diff -w out2 output.txt|| break

done

rm generator 
rm splitter
rm control
rm ../distributedv2.0/testing_distributed.out
