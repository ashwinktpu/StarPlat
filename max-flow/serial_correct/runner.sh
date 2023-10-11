#g++ -std=c++17 -g -I /usr/local/include/boost max_flow_boost.cpp -o testing_serial.out
g++ -std=c++17 -Wshadow -Wall -o testing_serial.out max_flow.cpp -g -fsanitize=address -fsanitize=undefined -D_GLIBCXX_DEBUG
./testing_serial.out small.txt 2> error.err 1>logfile.log
# ./testing_serial.out /home/cs22m028/Documents/CS22M028/testGraph/weightedGraphs/wikiwt.txt > logfile.log
# ./testing_serial.out /home/cs22m028/Documents/CS22M028/StarPlat/max-flow/serial_correct/testCases_small/small1.txt 2> logfile.log 1>error.err  
rm testing_serial.out