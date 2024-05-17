

#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

int main(void) {
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(8080);

  if (bind(server_socket, (struct sockaddr*) &addr, addr_len) == -1) {
    perror("Failed to bind socket\n");
    exit(EXIT_FAILURE);
  }

  if (listen(server_socket, 3) == -1) {
    perror("Failed to accept connection\n");
    exit(EXIT_FAILURE);
  }

  int client_socket;
  if ((client_socket = accept(server_socket, (struct sockaddr*) &addr, &addr_len)) == -1) {
    perror("Failed to accept connection\n");
    exit(EXIT_FAILURE);
  }

  char buffer[1024];
  strcpy(buffer, "Hello World!\n");
  if (send(client_socket, buffer, strlen(buffer), 0) == -1) {
    perror("Failed to send data to socket\n");
    exit(EXIT_FAILURE);
  }

  close(client_socket);
  close(server_socket);
  return 0;
}
