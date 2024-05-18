
#include "http.h"

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

char *create_http_response(char *status_code, char *data) {
  char *response = (char *) calloc(1024, sizeof(char));
  int offset = 0;

  // Create HTTP header
  offset += sprintf(response + offset, "HTTP/1.1 200 OK\r\n");
  offset += sprintf(response + offset, "Content-Type: text/html\r\n");
  offset += sprintf(response + offset, "\r\n");

  // Create HTML for response body
  offset += sprintf(response + offset, "<html><body><h1>%s</h1></body></html>", data);

  return response;
}




