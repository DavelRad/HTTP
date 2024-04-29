

#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

int main(void) {
  int server_sock = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in addr;
  int addr_len = sizeof(addr);
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(8080);

  bind(server_sock, (struct sockaddr*) &addr, addr_len);

  listen(server_sock, 3);

  int sock = accept(server_sock, (struct sockaddr*) &addr, &addr_len);

  char buffer[1024];
  strcpy(buffer, "Hello World!\n");
  send(sock, buffer, strlen(buffer), 0);

  close(sock);
  close(server_sock);
  return 0;
}
