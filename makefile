project: client server

client: client.c sharedmemory.c
	gcc -o client client.c sharedmemory.c

server: server.c sharedmemory.c
	gcc -o server server.c sharedmemory.c
	