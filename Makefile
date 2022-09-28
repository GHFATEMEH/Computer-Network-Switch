CC := g++ -g -std=c++11 

all: a.out System.out Switch.out

a.out: main.o functions.o
	$(CC) -o a.out main.o functions.o

System.out: System.o functions.o
	$(CC) -o System.out System.o functions.o

Switch.out: Switch.o functions.o
	$(CC) -o Switch.out Switch.o functions.o

main.o: main.cpp System.h Switch.h functions.h
	$(CC) -c main.cpp

System.o: System.cpp System.h functions.h
	$(CC) -c System.cpp

Switch.o: Switch.cpp Switch.h functions.h
	$(CC) -c Switch.cpp

functions.o: functions.cpp functions.h define.h
	$(CC) -c functions.cpp

.PHONY: clean
clean:
	rm *.o
	rm a.out
	rm System.out
	rm Switch.out