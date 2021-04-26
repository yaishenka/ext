/**
 * @file client.h
 * @author yaishenka
 * @date 13.03.2021
 * @brief Contains main loop
 */
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
#include "lseek_pos.h"
#include "utils.h"

#define HELP "help"
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
#define LSEEK "lseek"

#define command_buffer_lenght 256

void client(const char* path_to_fs_file) {
  char buffer[command_buffer_lenght];

  while (true) {
    read_command_from_stdin(buffer, command_buffer_lenght);
    char command[command_buffer_lenght];
    char* first_arg_pos = parse_command(buffer, command);
    if (strcmp(HELP, command) == 0) {
      printf("You are working with minifs\n"
             "Authored by yaishenka\n"
             "Source available ad github.com/yaishenka/ext\n"
             "Available commands:\n"
             "help -- print this text\n"
             "quit -- close program\n"
             "ls [path] -- list directory contents\n"
             "init -- init file system\n"
             "read_fs -- read fs_file and checks it\n"
             "mkdir [path] -- make directories\n"
             "touch [path] -- create files\n"
             "open [path] -- open file and return FD\n"
             "close [fd] -- close FD\n"
             "write [fd] [data] -- write data to FD\n"
             "write_from [fd] [path] -- read data from path and write to FD\n"
             "read [fd] [size] -- read size bytes from FD\n"
             "read_to [fd] [path] [size] -- read file from fd.pos and write data to path. "
             "If size not specified file will be readed till end\n"
             "lseek [fd] [pos] -- set fd.pos = pos\n");
    } else if (strcmp(INIT, command) == 0) {
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
      char* second_arg_position =
          parse_command(first_arg_pos, fd_to_write_text);
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
      char* second_arg_position =
          parse_command(first_arg_pos, fd_to_write_text);
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
      char* second_arg_position =
          parse_command(first_arg_pos, fd_to_write_text);
      uint16_t fd_to_write = strtol(fd_to_write_text, NULL, 10);

      if (second_arg_position == NULL || strlen(second_arg_position) == 0) {
        printf("Read to requires path\n");
        continue;
      }
      char path[command_buffer_lenght];
      char* third_argument_pos = parse_command(second_arg_position, path);

      if (third_argument_pos == NULL || strlen(third_argument_pos) == 0) {
        read_file_to_file(path_to_fs_file, fd_to_write, path, -1);
        continue;
      }

      char size_text[command_buffer_lenght];
      parse_command(third_argument_pos, size_text);
      uint32_t size = strtol(size_text, NULL, 10);
      read_file_to_file(path_to_fs_file, fd_to_write, path, size);
    } else if (strcmp(LSEEK, command) == 0) {
      if (first_arg_pos == NULL || strlen(first_arg_pos) == 0) {
        printf("Lseek requires fd\n");
        continue;
      }
      char fd_to_seek_text[command_buffer_lenght];
      char* second_arg_position = parse_command(first_arg_pos, fd_to_seek_text);
      uint16_t fd_to_seek = strtol(fd_to_seek_text, NULL, 10);

      if (second_arg_position == NULL || strlen(second_arg_position) == 0) {
        printf("Lseek requires position\n");
        continue;
      }

      char pos_text[command_buffer_lenght];
      parse_command(second_arg_position, pos_text);
      uint32_t pos = strtol(pos_text, NULL, 10);

      lseek_pos(path_to_fs_file, fd_to_seek, pos);
    } else {
      printf("Unsupported command\n");
    }
  }
}

#endif //EXT_FILESYSTEM_INTERFACE_CLIENT_H_


