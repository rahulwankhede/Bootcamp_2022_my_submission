CC=g++
CLIENT_FILE=myclient.c
SERVER_FILE=multithreaded_server.c
OPTS=-lpthread

all: client server

client: $(CLIENT_FILE)
	$(CC) $(CLIENT_FILE) -o client $(OPTS)

server: $(SERVER_FILE)
	$(CC) $(SERVER_FILE) -o server $(OPTS)

clean:
	rm -f client server
