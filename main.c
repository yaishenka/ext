#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "FileSystem/interface/client.h"

int main() {
  const char* fs_file_path = "test_fs";
  client(fs_file_path);
}
