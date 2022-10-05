CC = g++
CFLAGS = -g -Wall -std=c++20

OBJS = main.o sqlFunctions.o tables.o utils.o

all: $(OBJS)
	$(CC) $(CFLAGS) -o p1 *.o 
	./p1

clean:
	rm ./p1
	rm *.o 

main.o: main.cpp globals.h sqlFunctions.o tables.h
	$(CC) $(CFLAGS) -c main.cpp

sqlFunctions.o: sqlFunctions.cpp sqlFunctions.h globals.h
	$(CC) $(CFLAGS) -c sqlFunctions.cpp

tables.o: tables.cpp tables.h globals.h
	$(CC) $(CFLAGS) -c tables.cpp

utils.o: utils.cpp utils.h globals.h 
	$(CC) $(CFLAGS) -c utils.cpp