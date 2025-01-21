CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L -g

.PHONY: all clean

TARGETS = server client
HEADERS = http.h

all: $(TARGETS)

server: server.o
	$(CC) $(CFLAGS) -o $@ server.o

client: client.o
	$(CC) $(CFLAGS) -o $@ client.o

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGETS)