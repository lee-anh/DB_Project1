CC = g++
CFLAGS = -g -Wall -std=c++20

OBJS = main.o Database.o

all: $(OBJS)
	$(CC) $(CFLAGS) -o p1 *.o 
	./p1

clean:
	rm ./p1
	rm *.o 

main.o: main.cpp Database.h
	$(CC) $(CFLAGS) -c main.cpp

Database.o: Database.cpp Database.h 
	$(CC) $(CFLAGS) -c Database.cpp
