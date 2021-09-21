all:
#~ 	g++ -O3 sssp_dsl.cc -o sssp.out
#~ 	cat sssp_dsl.cc main.template.cu > main.cu
#~ 	nvcc main.cu -o sssp
	/usr/local/cuda-9.2/bin/nvcc -o "ssspMain".out "ssspMain.cu"  -std=c++14
	./ssspMain.out < input.txt
	./ssspMain.out < ../dataRecords/USA-road-d.NY.gr

cp:
