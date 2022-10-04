CC = g++
CFLAGS = -g -Wall -std=c++20

OBJS = main.o 

all: $(OBJS)
	$(CC) $(CFLAGS) -o p1 *.o 
	./p1

clean:
	rm ./p1
	rm *.o 

main.o: main.cpp globals.h tables.h
	$(CC) $(CFLAGS) -c main.cpp

tables.o: tables.cpp tables.Heuristic
	$(CC) $(CFLAGS) -c tables.cpp


