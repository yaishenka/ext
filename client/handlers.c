/** @author yaishenka
    @date 25.04.2021 */
#include "handlers.h"
#include "net_utils.h"
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <string.h>

bool receive_result(int sockd) {
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

  if (write_while(STDOUT_FILENO, data, size) == -1) {
    free(data);
    return false;
  }

  free(data);
  return true;
}

bool send_help_command(int sockd) {
  if (!send_command(sockd, Help)) {
    return false;
  }
  return receive_result(sockd);
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
  return receive_result(sockd);
}

bool send_mkdir_command(int sockd, const char* path) {
  if (!send_command(sockd, Mkdir)) {
    return false;
  }

  if (!send_data(sockd, path, strlen(path))) {
    return false;
  }

  return receive_result(sockd);
}

bool send_touch_command(int sockd, const char* path) {
  if (!send_command(sockd, Touch)) {
    return false;
  }

  if (!send_data(sockd, path, strlen(path))) {
    return false;
  }

  return receive_result(sockd);
}