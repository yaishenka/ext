/** @author yaishenka
    @date 25.04.2021 */
#include <stdio.h>
#include "client.h"


int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "Address should be in arguments. Using localhost:8000\n");
    const char* address = "localhost:8000";
    return client(address);
  }

  const char* address = argv[1];
  return client(address);
}