

all: server

clean: server_clean

server: ./server.c ./server.h ./http.c ./http.h
	gcc -D_REENTRANT -pthread -o ./server ./server.c -Wall -Werror

server_clean:
	rm -f ./server
