#g++ -std=c++17 main.cpp -o personal

g++ -std=c++17 graph_v_in.cpp -o generator
g++ -std=c++17 control.cpp -o control 
g++ -std=c++17 splitter.cpp -o splitter


for ((i=1000;i<=10000;i+=1000)) ; do

    ./generator $i >input.txt

    ./splitter 

    ./control input.txt > out2

    cd ../distributedv2.0
    bash runner.sh
    wait

    cd ../evaluation
    echo $i >> output.txt
#    diff -w out2 output.txt|| break

done

rm generator 
rm splitter
rm control
