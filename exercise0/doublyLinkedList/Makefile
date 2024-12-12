.PHONY: all clean

#Compiler und Compiler-Flags als Variable
#Aufgabe 5.
CC = gcc
CFLAGS = -std=c99 -Wall -pedantic

#Ziel
all: main

#verlinkt dateien zu einem ausführbaren
main: main.o list.o
	$(CC) $(CFLAGS) -o $@ $^

#regel für kompilieren von .c in .o 
#Aufgabe 6.
%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

#remove all .o files
clean:
	rm -f *.o main
