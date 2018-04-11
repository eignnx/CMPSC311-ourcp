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

# Build client using client.c and proj.h
client: client.c proj.h 
	$(COMPILER) $(FLAGS) client.c -o client

# Build server using server.c and proj.h
server: server.c proj.h
	$(COMPILER) $(FLAGS) server.c -o server

clean:
	rm -vf client
	rm -vf server

