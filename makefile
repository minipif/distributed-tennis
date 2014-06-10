all: server client

server: server.c server.h tennis.h
	gcc -Wall -s $< -o $@

client: client.c client.h tennis.h
	gcc -Wall -s $< -o $@

clean:
	rm -f server client *.o a.out *~

.PHONY: all clean

