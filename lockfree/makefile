CXX = g++

main: main.o adjacencylist.o mdlist.o
	$(CXX) -o main main.o adjacencylist.o mdlist.o

main.o: main.cpp structs.h mdlist.h mdlist.fwd.h adjacencylist.h adjacencylist.fwd.h
	$(CXX) -c main.cpp

adjacencylist.o: structs.h mdlist.h mdlist.fwd.h adjacencylist.h adjacencylist.fwd.h

mdlist.o: structs.h mdlist.h mdlist.fwd.h adjacencylist.h adjacencylist.fwd.h