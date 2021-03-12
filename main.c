#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "FileSystem/interface/client.h"

int main() {
  const char* fs_file_path = "test_fs";
  client(fs_file_path);
}

// TODO посмотреть сколько рекордов вообще можно писать в блок
// TODO протестить чтение если запись в больше чем один блок