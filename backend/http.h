
#ifndef HTTP_H
#define HTTP_H

#include <stdlib.h>
#include <stdio.h>

typedef struct HTTPRequest {
  char *method;
  char *uri;
  char *http_version;
  char *headers;
  char *body;
} HttpRequest;

HttpRequest parse_http_request(const char *request_str);


#endif