CC = g++
CFLAGS = -g -Wall -std=c++20

OBJS = main.o Database.o

all: $(OBJS)
	$(CC) $(CFLAGS) -o p1 *.o 
	./p1 1
	./p1 2 
	./p1 3

unordered: $(OBJS)
	$(CC) $(CFLAGS) -o p1 *.o 
	./p1 1

ordered: $(OBJS)
	$(CC) $(CFLAGS) -o p1 *.o 
	./p1 2

hashed: $(OBJS)
	$(CC) $(CFLAGS) -o p1 *.o 
	./p1 3

test: 
	cmake -S . -B build
	cmake --build build
	cd build && ctest --output-on-failure
	cd .. 

clean:
	rm ./p1
	rm *.o 

main.o: main.cpp Database.h
	$(CC) $(CFLAGS) -c main.cpp

Database.o: Database.cpp Database.h 
	$(CC) $(CFLAGS) -c Database.cpp

