#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "FileSystem/interface/client.h"

int main(int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "Path to fs file wasn't specified. Using default name!\n");
    const char* fs_file_path = "test_fs";
    client(fs_file_path);
  } else {
    const char* fs_file_path = argv[1];
    client(fs_file_path);
  }
}