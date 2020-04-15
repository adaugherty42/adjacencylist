CXX = g++

main: main.o adjacencylist.o mdlist.o
	$(CXX) -o main main.o adjacencylist.o mdlist.o -pthread

main.o: main.cpp structs.h adjacencylist.h mdlist.h
	$(CXX) -c main.cpp

adjacencylist.o: structs.h adjacencylist.h  mdlist.h

mdlist.o: structs.h adjacencylist.h mdlist.h