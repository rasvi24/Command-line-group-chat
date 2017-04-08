all: server client

server: ser.c
	gcc -o server ser.c -lpthread

client: cli.c
	gcc -o client cli.c -lncurses -lpthread

clean:
	rm -f server client