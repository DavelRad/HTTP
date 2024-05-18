
#ifndef HTTP_H
#define HTTP_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct HTTPRequest {
  char *method;
  char *uri;
  char *http_version;
  char *headers;
  char *body;
} HttpRequest;

HttpRequest parse_http_request(const char *request_str);
char *create_http_response(char *status_code, char *data);
char *handleGetRequest(HttpRequest request);
char *handlePostRequest(HttpRequest request);
char *handleDeleteRequest(HttpRequest request);


#endif
