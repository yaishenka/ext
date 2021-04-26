/** @author yaishenka
    @date 25.04.2021 */
#include "client.h"
#include "utils.h"
#include "net_utils.h"
#include "handlers.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>

int client(const char* address) {
  int sockd = make_connection(address);
  char buffer[command_buffer_lenght];
  printf("Connection made. socket: %d\n", sockd);
  while (true) {
    read_command_from_stdin(buffer, command_buffer_lenght);
    char command[command_buffer_lenght];
    char* first_arg_pos = parse_command(buffer, command);

    if (strcmp(HELP, command) == 0) {
      if (!send_help_command(sockd)) {
        fprintf(stderr, "Error while sending help command. Abort!\n");
        send_quit_command(sockd);
        break;
      }
    } else if (strcmp(QUIT, command) == 0) {
      if (!send_quit_command(sockd)) {
        fprintf(stderr, "Error while sending quit command. Restart server! Abort!\n");
        break;
      }
      break;
    } else if (strcmp(INIT, command) == 0) {
      if (!send_init_command(sockd)) {
        fprintf(stderr, "Error while sending init command. Abort!\n");
        send_quit_command(sockd);
        break;
      }
    } else if (strcmp(READ_FS, command) == 0) {
      if (!send_read_fs_command(sockd)) {
        fprintf(stderr, "Error while sending read_fs command. Abort!\n");
        send_quit_command(sockd);
        break;
      }
    } else if (strcmp(LS, command) == 0) {
      if (first_arg_pos == NULL || strlen(first_arg_pos) == 0) {
        printf("Ls requires path\n");
        continue;
      }
      char path[command_buffer_lenght];
      parse_command(first_arg_pos, path);
      if (!send_ls_command(sockd, path)) {
        fprintf(stderr, "Error while sending ls command. Abort!\n");
        send_quit_command(sockd);
        break;
      }
    } else if (strcmp(MKDIR, command) == 0) {
      if (first_arg_pos == NULL || strlen(first_arg_pos) == 0) {
        printf("Mkdir requires path\n");
        continue;
      }
      char path[command_buffer_lenght];
      parse_command(first_arg_pos, path);
      if (!send_mkdir_command(sockd, path)) {
        fprintf(stderr, "Error while sending mkdir command. Abort!\n");
        send_quit_command(sockd);
        break;
      }
    } else if (strcmp(TOUCH, command) == 0) {
      if (first_arg_pos == NULL || strlen(first_arg_pos) == 0) {
        printf("Mkdir requires path\n");
        continue;
      }
      char path[command_buffer_lenght];
      parse_command(first_arg_pos, path);
      if (!send_touch_command(sockd, path)) {
        fprintf(stderr, "Error while sending touch command. Abort!\n");
        send_quit_command(sockd);
        break;
      }
    } else {
        printf("Unknown command\n");
        continue;
    }
  }

  close_connection(sockd);
  return 0;
}



