#g++ -std=c++17 -g -I /usr/local/include/boost max_flow_boost.cpp -o testing_serial.out
g++ -std=c++17 -Wshadow -Wall -o testing_serial.out max_flow.cpp -g -fsanitize=address -fsanitize=undefined -D_GLIBCXX_DEBUG
./testing_serial.out small.txt
