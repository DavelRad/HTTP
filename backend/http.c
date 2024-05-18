
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
