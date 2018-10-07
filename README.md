# Systems-and-Distributed-Computing-Assignment-2
Assignment 2 for Systems and Distributing computing, client/server program to execute computationally expensive tasks on a multi-threaded system

## Shared Memory Structure

```
{
	active: int,
	query: unsigned long,
	query_status: char,
	result: unsigned long[10],
	result_status: char[10],
	progress: char[10],
}
```

## Client Server Communication

### New request

- User:   inputs data
- Client: wait for request_status = 0
- Client: request = user data
- Client: request_status = 1
- Client: wait for request_status = 0
- Client: read slot index

- Server: wait for request_status = 1
- Server: read request
- Server: allocate slot
- Server: request = slot index
- Server: request_status = 0

### Server return result

- Server: wait for result_status[i] = 0
- Server: result[i] = number
- Server: result_status[i] = 1

- Client: wait for result_status[i] = 1
- Client: read result[i]
- Client: result_status[i] = 0

### Server finishes a request

- Server: wait for result_status[i] = 0
- Server: result_status[i] = 2

- Client: if result_status[i] = 2
- Client: display finish and time taken

### Client prints progress

- Client print progress[]

## User Input (press ENTER to begin input)

- number: 32 bit ubsigned long number to be factorised
- '0': test mode whilst there are no outstanding queries
- 'q': quit the program