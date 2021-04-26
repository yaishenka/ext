/** @author yaishenka
    @date 25.04.2021 */
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "net_utils.h"

void parse_address(const char* address, char** host, long* port) {
  char* dots_position = strchr(address, ':');
  if (dots_position == NULL) {
    fprintf(stderr, "Address in wrong format\n");
    exit(EXIT_FAILURE);
  }

  *port = strtol(dots_position + 1, NULL, 10);

  size_t host_size = dots_position - address + 1;
  *host = calloc(host_size, sizeof(char));

  memcpy(*host, address, host_size - 1);
  (*host)[host_size - 1] = '\0';
}

int make_connection(const char* address) {
  long port = 0;
  char* host = NULL;
  parse_address(address, &host, &port);

  int sockd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockd == -1) {
    fprintf(stderr, "Can't create socket\n");
    free(host);
    exit(EXIT_FAILURE);
  }

  struct hostent* hostent = gethostbyname(host);
  in_addr_t* ip = (in_addr_t*) (*hostent->h_addr_list);

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_addr.s_addr = *ip;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);

  if (connect(sockd, (const struct sockaddr*) &addr, sizeof(struct sockaddr_in))
      == -1) {
    fprintf(stderr, "Can't connect\n");
    free(host);
    exit(EXIT_FAILURE);
  }

  free(host);
  return sockd;
}

void close_connection(int sockd) {
  close(sockd);
}

bool send_command(int sockd, enum Command command) {
  ssize_t written = write_while(sockd, (char*)&command, sizeof(enum Command));

  if (written != sizeof(enum Command)) {
    fprintf(stderr, "Can't write command to socket\n");
    return false;
  }

  return true;
}

bool send_data(int sockd, char* data, size_t size) {
  dprintf(sockd, "%zu", size);
  if (write_while(sockd, data, size) == -1) {
    return false;
  }

  return true;
}

bool receive_command(int fd, enum Command* command) {
  ssize_t read = read_while(fd, (char*)command, sizeof(enum Command));
  if (read != sizeof(enum Command)) {
    fprintf(stderr, "Can't read command from socket\n");
    return false;
  }

  return true;
}

bool receive_data(int sockd, char** data, size_t* size) {
  const size_t buffer_size = 1024;
  char buffer[buffer_size + 1];
  ssize_t readed = 0;
  ssize_t to_read = -1;
  ssize_t remain_to_read = -1;
  char* current_pointer_to_write = NULL;

  while (to_read == -1 || remain_to_read != 0) {
    ssize_t current_readed = 0;
    if (remain_to_read != -1) {
      current_readed = recv(sockd, buffer, remain_to_read, 0);
      readed += current_readed;
    } else {
      current_readed = recv(sockd, buffer, buffer_size, 0);
      readed += current_readed;
    }

    if (current_readed == -1) {
      fprintf(stderr, "Error while reading data from socket. Abort!\n");
      return false;
    }

    buffer[readed] = '\0';

    char* end = buffer;

    if (to_read == -1) {
      to_read = strtol(buffer, &end, 10);
      *data = calloc(to_read + 1, sizeof(char));
      (*data)[to_read] = '\0';
      current_pointer_to_write = *data;
      *size = to_read;
      remain_to_read = to_read;
    }

    current_readed -= (end - buffer);
    remain_to_read -= current_readed;

    memcpy(current_pointer_to_write, end, current_readed);
    current_pointer_to_write += current_readed;
  }

  return true;
}