
#include "server.h"
#include "http.c"

int main(void) {
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(8080);

  if (bind(server_socket, (struct sockaddr *) &addr, addr_len) == -1) {
    perror("Failed to bind socket\n");
    exit(EXIT_FAILURE);
  }

  // Set max number of connections that we can handle
  if (listen(server_socket, MAX_NUM_THREADS) == -1) {
    perror("Failed to start accepting connections\n");
    exit(EXIT_FAILURE);
  }

  // Accept connections
  while (true) {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    int client_socket;
    if ((client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &client_addr_len)) == -1) {
      perror("Failed to accept connection\n");
    }
    else {
      // Spawn thread to handle connection
      pthread_t thread;
      connection_args *args = (connection_args *) malloc(sizeof(connection_args));
      args->client_socket = client_socket;
      pthread_create(&thread, NULL, handleConnection, (void *) args);
    }
  }


  close(server_socket);
  return 0;
}


void *handleConnection(void *arg) {
  // Convert thread args
  connection_args *my_args = (connection_args *) arg;
  int client_socket = my_args->client_socket;
  free(my_args);

  // Read request from socket
  char buffer[1024];
  ssize_t bytes_read = recv(client_socket, buffer, 1024, 0);
  if (bytes_read < 0) {
    perror("Failed to read from socket");
    close(client_socket);
    pthread_exit(NULL);
  }
  buffer[bytes_read] = '\0';

  // Parse HTTP request
  HttpRequest request = parse_http_request(buffer);

  // Send Response
  char *response;
  if (strlen(request.uri) > 0) {
    response = create_http_response("200 OK", request.uri);
  }
  else {
    response = create_http_response("404 Not Found", "404 Not Found");
  }

  send(client_socket, response, strlen(response), 0);
  free(response);


  // Exit Thread
  close(client_socket);
  pthread_exit(NULL);
}
