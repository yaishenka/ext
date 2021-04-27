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
        fprintf(stderr,
                "Error while sending quit command. Restart server! Abort!\n");
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
        printf("Touch requires path\n");
        continue;
      }
      char path[command_buffer_lenght];
      parse_command(first_arg_pos, path);
      if (!send_touch_command(sockd, path)) {
        fprintf(stderr, "Error while sending touch command. Abort!\n");
        send_quit_command(sockd);
        break;
      }
    } else if (strcmp(OPEN, command) == 0) {
      if (first_arg_pos == NULL || strlen(first_arg_pos) == 0) {
        printf("Open requires path\n");
        continue;
      }
      char path[command_buffer_lenght];
      parse_command(first_arg_pos, path);
      if (!send_open_command(sockd, path)) {
        fprintf(stderr, "Error while sending open command. Abort!\n");
        send_quit_command(sockd);
        break;
      }
    } else if (strcmp(CLOSE, command) == 0) {
      if (first_arg_pos == NULL || strlen(first_arg_pos) == 0) {
        printf("Close requires fd\n");
        continue;
      }
      char fd_to_close_text[command_buffer_lenght];
      parse_command(first_arg_pos, fd_to_close_text);
      uint16_t fd_to_close = strtol(fd_to_close_text, NULL, 10);

      if (!send_close_command(sockd, fd_to_close)) {
        fprintf(stderr, "Error while sending close command. Abort!\n");
        send_quit_command(sockd);
        break;
      }
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

      if (!send_lseek_command(sockd, fd_to_seek, pos)) {
        fprintf(stderr, "Error while sending lseek command. Abort!\n");
        send_quit_command(sockd);
        break;
      }
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
      if (!send_write_command(sockd, fd_to_write, data, strlen(data))) {
        fprintf(stderr, "Error while sending write command. Abort!\n");
        send_quit_command(sockd);
        break;
      }
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

      if (!send_read_command(sockd, fd_to_read, size)) {
        fprintf(stderr, "Error while sending read command. Abort!\n");
        send_quit_command(sockd);
        break;
      }
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

      if (!send_write_from_command(sockd, fd_to_write, path)) {
        fprintf(stderr, "Error while sending write from command. Abort!\n");
        send_quit_command(sockd);
        break;
      }
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
        printf("Read to requires size\n");
        continue;
      }

      char size_text[command_buffer_lenght];
      parse_command(third_argument_pos, size_text);
      uint32_t size = strtol(size_text, NULL, 10);
      if (!send_read_to_command(sockd, fd_to_write, path, size)) {
        fprintf(stderr, "Error while sending read to command. Abort!\n");
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



