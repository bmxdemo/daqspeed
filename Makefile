test.out: src/main.cpp Makefile
	export OMP_NUM_THREADS=1;
	g++ -fopenmp -Ofast -o test.out src/main.cpp
	g++ -fopenmp -Ofast -o ampPhase.out src/ampPhase.cpp
