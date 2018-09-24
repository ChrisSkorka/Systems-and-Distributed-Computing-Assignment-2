# /////////////////////////////////////////////////////////////////////////////
# Filename:        	makefile
# Author:          	Christopher Skorka
# Date Created:    	10/09/2018
# Description:     	makes the client and server executables
# Excecutables:		client
#					server
# Sources:			client.cpp 
#					server.cpp
#					sharedmemory.cpp 
#					sharedmemory.hpp 
#					threadpool.cpp 
#					threadpool.hpp
# /////////////////////////////////////////////////////////////////////////////

project: client server

client: client.cpp sharedmemory.cpp sharedmemory.hpp threadpool.cpp threadpool.hpp
	gcc -o client client.cpp sharedmemory.cpp threadpool.cpp -lpthread

server: server.cpp sharedmemory.cpp sharedmemory.hpp threadpool.cpp threadpool.hpp
	gcc -o server server.cpp sharedmemory.cpp threadpool.cpp -lpthread
	