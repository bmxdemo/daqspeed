test.out: src/main.cpp Makefile
	g++ -fopenmp -Ofast -o test.out src/main.cpp
