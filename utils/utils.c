/** @author yaishenka
    @date 10.03.2021 */
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "utils.h"

int buffered_write(char** buffer, size_t* buffer_size, char* data, size_t size) {
  if (*buffer == NULL) {
    *buffer = calloc(size, sizeof(char));
  } else {
    *buffer = realloc(*buffer, *buffer_size + size);
  }

  memcpy(*buffer + *buffer_size, data, size);
  *buffer_size += size;

  return size;
}

int read_while(const int fd, char* buffer, size_t to_read) {
  size_t total = 0;

  while (total != to_read) {
    ssize_t readed = read(fd, buffer + total, to_read - total);
    if (readed == -1) {
      return readed;
    }

    total += readed;
  }

  return total;
}

int write_while(const int fd, const char* buffer, size_t to_write) {
  size_t total = 0;

  while (total != to_write) {
    ssize_t written = write(fd, buffer + total, to_write - total);

    if (written == -1) {
      return written;
    }

    total += written;
  }

  return total;
}

char* parse_path(const char* path, char* current_file_name) {
  if (strcmp(path, "/") == 0) {
    strcpy(current_file_name, "/");
    return NULL;
  }

  char* first_slash_position = strchr(path, '/');

  if (first_slash_position == NULL) {
    fprintf(stderr, "Incorrect path in parse_path function. Abort!\n");
    exit(EXIT_FAILURE);
  }

  uint16_t path_length = strlen(path);

  char* second_slash_position = strchr(first_slash_position + 1, '/');
  uint16_t current_file_name_length = 0;

  if (second_slash_position == NULL) {
    current_file_name_length = path_length - 1;
  } else {
    current_file_name_length = second_slash_position - first_slash_position - 1;
  }

  if (current_file_name_length == 0) {
    fprintf(stderr, "Incorrect path in parse_path function. Abort!\n");
    exit(EXIT_FAILURE);
  }

  memcpy(current_file_name, first_slash_position + 1, current_file_name_length);
  current_file_name[current_file_name_length + 1] = '\0';

  return second_slash_position;
}

bool split_path(const char* path, char* parent_path, char* dirname) {
  uint16_t path_length = strlen(path);

  char buffer[buffer_length];
  memcpy(buffer, path, path_length);
  if (buffer[path_length - 1] != '/') {
    buffer[path_length] = '\0';
  } else {
    buffer[path_length - 1] = '\0';
  }
  path_length = strlen(buffer);

  if (buffer[0] != '/' || path_length == 0) {
    fprintf(stderr, "Incorrect path to split. Abort!\n");
    return false;
  }

  char* slash_pos = strchr(buffer + 1, '/');
  char* last_slash_pos = buffer;

  while (slash_pos != NULL) {
    last_slash_pos = slash_pos;
    slash_pos = strchr(slash_pos + 1, '/');
  }

  memcpy(parent_path, buffer, last_slash_pos - buffer + 1);
  parent_path[last_slash_pos - buffer + 1] = '\0';
  memcpy(dirname, last_slash_pos + 1, buffer_length - strlen(parent_path));

  if (strlen(dirname) == 0) {
    return false;
  }

  return true;
}

void read_command_from_stdin(char* command_buffer, size_t buffer_size) {
  fgets(command_buffer, buffer_size, stdin);
  uint32_t lenght = strlen(command_buffer);
  command_buffer[lenght - 1] = '\0';
}

char* parse_command(char* command_buffer, char* command) {
  char* first_space = strchr(command_buffer, ' ');

  if (first_space == NULL) {
    strcpy(command, command_buffer);
  } else {
    uint16_t command_length = first_space - command_buffer;
    memcpy(command, command_buffer, command_length);
    command[command_length] = '\0';
  }

  return first_space == NULL ? NULL : first_space + 1;
}

ssize_t get_file_size(const char* path_to_file) {
  int fd = open(path_to_file, O_RDONLY);
  if (fd == -1) {
    fprintf(stderr, "Can't open file. Abort!");
    return -1;
  }

  FILE* file = fdopen(fd, "rb");

  if (file == NULL) {
    fprintf(stderr, "Can't open file. Abort!");
    close(fd);
    return -1;
  }

  struct stat stat;
  if (fstat(fd, &stat) == -1) {
    fprintf(stderr, "Can't open file. Abort!");
    fclose(file);
    close(fd);
    return -1;
  }

  ssize_t size = stat.st_size;
  fclose(file);
  close(fd);
  return size;
}
