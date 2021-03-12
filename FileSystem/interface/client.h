/** @author yaishenka
    @date 12.03.2021 */
#ifndef EXT_FILESYSTEM_INTERFACE_CLIENT_H_
#define EXT_FILESYSTEM_INTERFACE_CLIENT_H_
#include <unistd.h>
#include <string.h>
#include "init.h"
#include "ls.h"
#include "create_dir.h"
#include "create_file.h"
#include "open_file.h"
#include "close_file.h"
#include "write_to_file.h"
#include "read_file.h"
#include "../utils.h"

#define LS "ls"
#define INIT "init"
#define READ_FS "read_fs"
#define MKDIR "mkdir"
#define TOUCH "touch"
#define OPEN "open"
#define QUIT "quit"
#define CLOSE "close"
#define WRITE "write"
#define WRITE_FROM "write_from"
#define READ "read"
#define READ_TO "read_to"

#define command_buffer_lenght 256

void client(const char* path_to_fs_file) {
  char buffer[command_buffer_lenght];

  while (true) {
    read_command_from_stdin(buffer, command_buffer_lenght);
    char command[command_buffer_lenght];
    char* first_arg_pos = parse_command(buffer, command);

    if (strcmp(INIT, command) == 0) {
      printf("Initializing fs\n");
      init_fs(path_to_fs_file);
    } else if (strcmp(READ_FS, command) == 0) {
      printf("Reading fs\n");
      read_fs(path_to_fs_file);
    } else if (strcmp(LS, command) == 0) {
      if (first_arg_pos == NULL || strlen(first_arg_pos) == 0) {
        printf("Ls requires path\n");
        continue;
      }

      char path[command_buffer_lenght];
      parse_command(first_arg_pos, path);
      ls(path_to_fs_file, path);
    } else if (strcmp(QUIT, command) == 0) {
      return;
    } else if (strcmp(MKDIR, command) == 0) {
      if (first_arg_pos == NULL || strlen(first_arg_pos) == 0) {
        printf("Mkdir requires path\n");
        continue;
      }

      char path[command_buffer_lenght];
      parse_command(first_arg_pos, path);
      create_dir(path_to_fs_file, path);
    } else if (strcmp(TOUCH, command) == 0) {
      if (first_arg_pos == NULL || strlen(first_arg_pos) == 0) {
        printf("Mkdir requires path\n");
        continue;
      }

      char path[command_buffer_lenght];
      parse_command(first_arg_pos, path);
      create_file(path_to_fs_file, path);
    } else if (strcmp(OPEN, command) == 0) {
      if (first_arg_pos == NULL || strlen(first_arg_pos) == 0) {
        printf("Open requires path\n");
        continue;
      }

      char path[command_buffer_lenght];
      parse_command(first_arg_pos, path);
      open_file(path_to_fs_file, path);
    } else if (strcmp(CLOSE, command) == 0) {
      if (first_arg_pos == NULL || strlen(first_arg_pos) == 0) {
        printf("Open requires path\n");
        continue;
      }
      char fd_to_close_text[command_buffer_lenght];
      parse_command(first_arg_pos, fd_to_close_text);
      uint16_t fd_to_close = strtol(fd_to_close_text, NULL, 10);

      close_file(path_to_fs_file, fd_to_close);
    } else if (strcmp(WRITE, command) == 0) {
      if (first_arg_pos == NULL || strlen(first_arg_pos) == 0) {
        printf("Write requires fd\n");
        continue;
      }

      char fd_to_write_text[command_buffer_lenght];
      char* second_arg_position = parse_command(first_arg_pos, fd_to_write_text);
      uint16_t fd_to_write = strtol(fd_to_write_text, NULL, 10);

      if (second_arg_position == NULL || strlen(second_arg_position) == 0) {
        printf("Write requires data\n");
        continue;
      }
      char data[command_buffer_lenght];
      parse_command(second_arg_position, data);

      write_to_file(path_to_fs_file, fd_to_write, data, strlen(data));
    } else if (strcmp(READ, command) == 0) {
      char fd_to_read_text[command_buffer_lenght];
      char* second_arg_position = parse_command(first_arg_pos, fd_to_read_text);
      uint16_t fd_to_read = strtol(fd_to_read_text, NULL, 10);

      if (second_arg_position == NULL || strlen(second_arg_position) == 0) {
        printf("Read requires size\n");
        continue;
      }

      char size_to_read_text[command_buffer_lenght];
      parse_command(second_arg_position, size_to_read_text);
      uint32_t size = strtol(size_to_read_text, NULL, 10);

      char data[command_buffer_lenght];

      read_file(path_to_fs_file, fd_to_read, data, size);
      data[size] = '\0';
      printf("Readed: %s\n", data);
    } else if (strcmp(WRITE_FROM, command) == 0) {
      if (first_arg_pos == NULL || strlen(first_arg_pos) == 0) {
        printf("Write from requires fd\n");
        continue;
      }

      char fd_to_write_text[command_buffer_lenght];
      char* second_arg_position = parse_command(first_arg_pos, fd_to_write_text);
      uint16_t fd_to_write = strtol(fd_to_write_text, NULL, 10);

      if (second_arg_position == NULL || strlen(second_arg_position) == 0) {
        printf("Write from requires path\n");
        continue;
      }
      char path[command_buffer_lenght];
      parse_command(second_arg_position, path);
      write_to_file_from_file(path_to_fs_file, fd_to_write, path);
    } else if (strcmp(READ_TO, command) == 0) {
      if (first_arg_pos == NULL || strlen(first_arg_pos) == 0) {
        printf("Read to requires fd\n");
        continue;
      }

      char fd_to_write_text[command_buffer_lenght];
      char* second_arg_position = parse_command(first_arg_pos, fd_to_write_text);
      uint16_t fd_to_write = strtol(fd_to_write_text, NULL, 10);

      if (second_arg_position == NULL || strlen(second_arg_position) == 0) {
        printf("Read to requires path\n");
        continue;
      }
      char path[command_buffer_lenght];
      parse_command(second_arg_position, path);
      read_file_to_file(path_to_fs_file, fd_to_write, path);
    } else {
      printf("Unsupported command\n");
    }
  }
}
/*
close /main
open /main
opened fd: 0
read 0 5
Total readed: 3
Readed: kek
close /main
open /main
opened fd: 0
write 0 123456789kek
Total written: 12
close 0
open /main
File doesn't exist. Abort!
ls /
.
ls /
ls /
quit
 */


#endif //EXT_FILESYSTEM_INTERFACE_CLIENT_H_


