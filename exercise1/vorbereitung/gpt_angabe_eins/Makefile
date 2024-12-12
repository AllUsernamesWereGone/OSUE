.PHONY: all clean

#Compiler und Flags als Variable
CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L
LDFLAGS = -pthread -lrt

#Ziel
all: supervisor generator 

#verlinken 
supervisor: supervisor.o common.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
generator: generator.o common.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)


#regel fürs kompilieren, header files und c files 
%.o: %.c %.h
	$(CC) $(CFLAGS) -g -c -o $@ $<

#remove files
clean:
	rm -f *.o supervisor generator

