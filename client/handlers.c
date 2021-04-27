/** @author yaishenka
    @date 25.04.2021 */
#include "handlers.h"
#include "net_utils.h"
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>

bool receive_result(int sockd, int output_fd) {
  size_t size = 0;
  char* data = NULL;
  if (!receive_data(sockd, &data, &size)) {
    if (data != NULL) {
      free(data);
    }

    return false;
  }

  if (data == NULL) {
    return false;
  }

  if (write_while(output_fd, data, size) == -1) {
    free(data);
    return false;
  }

  free(data);
  return true;
}

bool receive_read_result(int sockd, char** readed_data, size_t* readed) {
  size_t size = 0;
  char* data = NULL;
  if (!receive_data(sockd, &data, &size)) {
    if (data != NULL) {
      free(data);
    }

    return false;
  }

  char* data_position = NULL;
  *readed = strtol(data, &data_position, 10);
  data_position += 1;
  *readed_data = calloc(*readed + 1, sizeof(char));
  (*readed_data)[*readed] = '\0';
  memcpy(*readed_data, data_position, *readed);

  free(data);
  return true;
}

bool send_help_command(int sockd) {
  if (!send_command(sockd, Help)) {
    return false;
  }
  return receive_result(sockd, STDOUT_FILENO);
}

bool send_quit_command(int sockd) {
  return send_command(sockd, Quit);
}

bool send_init_command(int sockd) {
  return send_command(sockd, Init);
}

bool send_read_fs_command(int sockd) {
  return send_command(sockd, ReadFs);
}

bool send_ls_command(int sockd, const char* path) {
  if (!send_command(sockd, Ls)) {
    return false;
  }
  if (!send_data(sockd, path, strlen(path))) {
    return false;
  }
  return receive_result(sockd, STDOUT_FILENO);
}

bool send_mkdir_command(int sockd, const char* path) {
  if (!send_command(sockd, Mkdir)) {
    return false;
  }

  if (!send_data(sockd, path, strlen(path))) {
    return false;
  }

  return receive_result(sockd, STDOUT_FILENO);
}

bool send_touch_command(int sockd, const char* path) {
  if (!send_command(sockd, Touch)) {
    return false;
  }

  if (!send_data(sockd, path, strlen(path))) {
    return false;
  }

  return receive_result(sockd, STDOUT_FILENO);
}

bool send_open_command(int sockd, const char* path) {
  if (!send_command(sockd, Open)) {
    return false;
  }

  if (!send_data(sockd, path, strlen(path))) {
    return false;
  }

  return receive_result(sockd, STDOUT_FILENO);
}

bool send_close_command(int sockd, int fd) {
  if (!send_command(sockd, Close)) {
    return false;
  }

  char string_buffer[1024];
  size_t string_size = sprintf(string_buffer, "\t%d", fd);

  if (!send_data(sockd, string_buffer, strlen(string_buffer))) {
    return false;
  }

  return receive_result(sockd, STDOUT_FILENO);
}

bool send_lseek_command(int sockd, int opened_fd, int size) {
  if (!send_command(sockd, Lseek)) {
    return false;
  }

  char string_buffer[1024];
  size_t string_size = sprintf(string_buffer, "\t%d\t%d", opened_fd, size);

  if (!send_data(sockd, string_buffer, strlen(string_buffer))) {
    return false;
  }

  return receive_result(sockd, STDOUT_FILENO);
}

bool send_write_command(int sockd, int fd, char* data, size_t size) {
  if (!send_command(sockd, Write)) {
    return false;
  }

  char string_buffer[size + 128];
  size_t string_size = sprintf(string_buffer, "\t%d\t%zu\t%s", fd, size, data);
  if (!send_data(sockd, string_buffer, strlen(string_buffer))) {
    return false;
  }

  return receive_result(sockd, STDOUT_FILENO);
}

bool send_read_command(int sockd, int fd, size_t size) {
  if (!send_command(sockd, Read)) {
    return false;
  }

  char string_buffer[1024];
  size_t string_size = sprintf(string_buffer, "\t%d\t%zu", fd, size);

  if (!send_data(sockd, string_buffer, strlen(string_buffer))) {
    return false;
  }

  char* data = NULL;
  size_t readed = 0;

  bool result = receive_read_result(sockd, &data, &readed);

  if (result == false) {
    return false;
  }

  printf("Total read:%zu\n%s\n", readed, data);

  free(data);
  return result;
}

bool send_write_from_command(int sockd, int fd, const char* path) {
  ssize_t size = get_file_size(path);
  if (size == -1) {
    return false;
  }

  int descriptor = open(path, O_RDONLY);
  if (descriptor == -1) {
    fprintf(stderr, "Can't read file with data. Abort!\n");
    return false;
  }

  char* buffer = (char*) calloc(size, sizeof(char));
  size = read_while(descriptor, buffer, size);

  close(descriptor);

  if (size == -1) {
    fprintf(stderr, "Can't read file with data. Abort!\n");
    return false;
  }

  return send_write_command(sockd, fd, buffer, size);
}

bool send_read_to_command(int sockd, int fd, const char* path, size_t size) {
  if (!send_command(sockd, Read)) {
    return false;
  }

  char string_buffer[1024];
  size_t string_size = sprintf(string_buffer, "\t%d\t%zu", fd, size);

  if (!send_data(sockd, string_buffer, strlen(string_buffer))) {
    return false;
  }

  char* data = NULL;
  size_t readed = 0;

  bool result = receive_read_result(sockd, &data, &readed);

  if (result == false) {
    return false;
  }

  int fd_to_write = open(path, O_RDWR | O_CREAT);
  if (fd_to_write == -1) {
    fprintf(stderr, "Can't open file to write. Abort!\n");
    return false;
  }

  if (write_while(fd_to_write, data, readed) == -1) {
    fprintf(stderr, "Can't write to file to write. Abort!\n");
    free(data);
    return false;
  }

  free(data);

  return true;
}