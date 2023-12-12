graphs=germanyudwt.txt

echo $graphs

# g++ -std=c++17 -Wshadow -Wall -g -fsanitize=address -fsanitize=undefined -D_GLIBCXX_DEBUG  init_maxflow.cpp -o main.out 
# mpic++ -std=c++17 -Wshadow -Wall -o testing_distributed.out rma_

#g++ -std=c++17 -g -I /usr/local/include/boost max_flow_boost.cpp -o testing_serial.out

mpic++ -std=c++17 -Wshadow -Wall -o testing_distributed.out init_maxflow.cpp -g -fsanitize=address -fsanitize=undefined -D_GLIBCXX_DEBUG
mpirun -np 4 testing_distributed.out 1> logger.log 2> error.err

rm testing_distributed.out
