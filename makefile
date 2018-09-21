project: client server

client: client.c sharedmemory.c sharedmemory.h
	gcc -o client client.c sharedmemory.c

server: server.c sharedmemory.c sharedmemory.h
	gcc -o server server.c sharedmemory.c
	