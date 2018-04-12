#
# To compile both client and server, run
#     $ make all
# or just
#    $ make
#
# To compile just the server or just the client, run
#    $ make server
# or
#    $ make client
#
# To remove all executable files, run
#    $ make clean
 
# Define parameters
COMPILER=gcc
FLAGS=-Wall -std=c99

# Build both client and server
all: client server

# Build client using client.c and proj.h and object files
client: client.c proj.h sendrecv.o
	$(COMPILER) $(FLAGS) client.c sendrecv.o -o client

# Build server using server.c and proj.h and object files
server: server.c proj.h sendrecv.o
	$(COMPILER) $(FLAGS) server.c sendrecv.o -o server

# Compile sendrecv.c to object file
sendrecv.o: sendrecv.c
	$(COMPILER) $(FLAGS) -c $^

# Deletes all executables and object files.
clean:
	rm -vf client
	rm -vf server
	rm -vf *.o

