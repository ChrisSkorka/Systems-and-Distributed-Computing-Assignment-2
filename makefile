project: client server

client: client.cpp sharedmemory.cpp sharedmemory.hpp
	gcc -o client client.cpp sharedmemory.cpp -lpthread

server: server.cpp sharedmemory.cpp sharedmemory.hpp
	gcc -o server server.cpp sharedmemory.cpp -lpthread
	