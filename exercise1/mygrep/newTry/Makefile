.PHONY: all clean

#Compiler und Flags als Variable
CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L

#Ziel
all: mygrep 

#verlinken 
mygrep: main.o mygrep.o
	$(CC) $(CFLAGS) -o $@ $^

#regel fürs kompilieren, header files und c files 
%.o: %.c 
	$(CC) $(CFLAGS) -g -c -o $@ $<

#remove files
clean:
	rm -f *.o mygrep 

