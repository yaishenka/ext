/**
 * @file commands.h
 * @author yaishenka
 * @date 25.04.2021
 */
#ifndef EXT_UTILS_COMMANDS_H_
#define EXT_UTILS_COMMANDS_H_

#include <stdint.h>

enum Command {
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
