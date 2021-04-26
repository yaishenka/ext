/** @author yaishenka
    @date 26.04.2021 */
#include <stdio.h>
#include <stdlib.h>
#include "server/server.h"


int main(int argc, char** argv) {
  long port = 8000;
  if (argc == 1) {
    fprintf(stderr, "You should provide port and path to fs file as arguments. Using default 8000 port. Using default name\n");
    const char* fs_file_path = "test_fs";
    return server_loop(8000, fs_file_path);
  }

  if (argc == 2) {
    const char* fs_file_path = argv[1];
    fprintf(stderr, "You should provide port as argument. Using default 8000 port\n");
    return server_loop(8000, fs_file_path);
  }

  const char* fs_file_path = argv[1];
  port = strtol(argv[2], NULL, 10);
  return server_loop(port, fs_file_path);
}