
#include "http.h"

HttpRequest parse_http_request(const char *request_str) {
  HttpRequest request = {NULL, NULL, NULL, NULL, NULL};
  char buffer[1024];
  strncpy(buffer, request_str, 1023);
  buffer[1023] = '\0'; // Ensure null termination

  char *line = strtok(buffer, "\r\n");
  if (line) {
    request.method = strdup(strtok(line, " "));
    request.uri = strdup(strtok(NULL, " "));
    request.http_version = strdup(strtok(NULL, " "));
  }

  // Adjust this to dynamically allocate and copy headers/body if needed
  request.headers = strdup(strstr(request_str, "\r\n") + 2);
  char *header_end = strstr(request.headers, "\r\n\r\n");
  if (header_end) {
      *header_end = '\0'; // Null-terminate the headers
      request.body = strdup(header_end + 4); // Copy body
  }

  return request;
}

char *create_http_response(char *status_code, char *data) {
  char *response = (char *) calloc(1024, sizeof(char));
  int offset = 0;

  // Create HTTP header
  offset += sprintf(response + offset, "HTTP/1.1 %s\r\n", status_code);
  offset += sprintf(response + offset, "Content-Type: text/html\r\n");
  offset += sprintf(response + offset, "\r\n");

  // Create HTML for response body
  offset += sprintf(response + offset, "<html><body><h1>%s</h1></body></html>", data);

  return response;
}




