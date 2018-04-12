all: client
	./client

client: client.c
	gcc -pthread -o client client.c

clean:
	rm client
