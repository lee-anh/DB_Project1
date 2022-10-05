CC = g++
CFLAGS = -g -Wall -std=c++20

OBJS = main.o utils.o

all: $(OBJS)
	$(CC) $(CFLAGS) -o p1 *.o 
	./p1

clean:
	rm ./p1
	rm *.o 

main.o: main.cpp utils.h
	$(CC) $(CFLAGS) -c main.cpp

utils.o: utils.cpp utils.h 
	$(CC) $(CFLAGS) -c utils.cpp

