
#include "server.h"

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

HttpRequest parse_http_request(const char *request_str) {
    HttpRequest request;
    request.method = NULL;
    request.uri = NULL;
    request.http_version = NULL;
    request.headers = NULL;
    request.body = NULL;

    char buffer[1024];
    strncpy(buffer, request_str, 1024);
    char *line = strtok(buffer, "\r\n");

    request.method = strtok(line, " ");
    request.uri = strtok(NULL, " ");
    request.http_version = strtok(NULL, " ");

    request.headers = strstr(request_str, "\r\n") + 2;
    char *header_end = strstr(request.headers, "\r\n\r\n");
    if (header_end != NULL) {
        *header_end = '\0'; // Null-terminate the headers
        request.body = header_end + 4; // Start of body
    }

    return request;
}

void *handleConnection(void *arg) {
  connection_args *my_args = (connection_args *) arg;
  int client_socket = my_args->client_socket;

  char buffer[1024];
  ssize_t bytes_read = recv(client_socket, buffer, 1024 - 1, 0);
  if (bytes_read < 0) {
    perror("Failed to read from socket");
    close(client_socket);
    free(my_args);
    pthread_exit(NULL);
  }
  buffer[bytes_read] = '\0';

  HttpRequest request = parse_http_request(buffer);

  char response[1024];
  if (strcmp(request.uri, "/hello") == 0) {
      sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Hello from %s!</h1></body></html>", request.uri);
  } else {
      strcpy(response, "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>");
  }
  send(client_socket, response, strlen(response), 0);

  close(client_socket);
  free(my_args);
  pthread_exit(NULL);
}
