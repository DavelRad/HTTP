
#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>

#define MAX_NUM_THREADS 2

typedef struct connection_args {
  int client_socket;
} connection_args;

void *handleConnection(void *arg);


#endif
