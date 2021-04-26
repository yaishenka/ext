/**
 * @file commands.h
 * @author yaishenka
 * @date 25.04.2021
 */
#ifndef EXT_UTILS_COMMANDS_H_
#define EXT_UTILS_COMMANDS_H_

#include <stdint.h>

//#define HELP "help"
//#define LS "ls"
//#define INIT "init"
//#define READ_FS "read_fs"
//#define MKDIR "mkdir"
//#define TOUCH "touch"
//#define OPEN "open"
//#define QUIT "quit"
//#define CLOSE "close"
//#define WRITE "write"
//#define WRITE_FROM "write_from"
//#define READ "read"
//#define READ_TO "read_to"
//#define LSEEK "lseek"

enum Command: uint8_t {
  Help = 1,
  Ls,
  Init,
  ReadFs,
  Mkdir,
  Touch,
  Open,
  Quit,
  Close,
  Write,
  WriteFrom,
  Read,
  ReadTo,
  Lseek
};

#endif //EXT_UTILS_COMMANDS_H_
