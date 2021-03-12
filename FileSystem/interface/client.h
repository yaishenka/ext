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
#include "../utils.h"

#define LS "ls"
#define INIT "init"
#define READ_FS "read_fs"
#define MKDIR "mkdir"
#define TOUCH "touch"
#define OPEN "open"
#define QUIT "quit"

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
    } else {
      printf("Unsupported command\n");
    }
  }
}


#endif //EXT_FILESYSTEM_INTERFACE_CLIENT_H_


