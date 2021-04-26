/**
 * @file client.h
 * @author yaishenka
 * @date 25.04.2021
 * @bried Main cycle for client
 */
#ifndef EXT_CLIENT_CLIENT_H_
#define EXT_CLIENT_CLIENT_H_


#define HELP "help"
#define QUIT "quit"
#define INIT "init"
#define READ_FS "read_fs"
#define LS "ls"
#define MKDIR "mkdir"
#define TOUCH "touch"
#define OPEN "open"
#define CLOSE "close"
#define WRITE "write"
#define WRITE_FROM "write_from"
#define READ "read"
#define READ_TO "read_to"
#define LSEEK "lseek"

#define command_buffer_lenght 256

/**
 * Main cycle for client
 * @param address
 * @return
 */
int client(const char* address);

#endif //EXT_CLIENT_CLIENT_H_
