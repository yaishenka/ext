/**
 * @file handlers.h
 * @author yaishenka
 * @date 25.04.2021
 * @brief Contains handlers for commands from server
 */
#ifndef EXT_CLIENT_HANDLERS_H_
#define EXT_CLIENT_HANDLERS_H_

#include <stdbool.h>
#include <stdlib.h>

/**
 * Read count of lines and lines from socket
 * Prixnt result to stdout
 * @param sockd
 */
bool receive_result(int sockd, int output_fd);

/**
 * Send help command and wait for answer
 * @param sockd
 */
bool send_help_command(int sockd);

/**
 * Send quit command
 * @param sockd
 */
bool send_quit_command(int sockd);

/**
 * Send init command
 * @param sockd
 */
bool send_init_command(int sockd);

/**
 * Send read fs command
 * @param sockd
 * @return
 */
bool send_read_fs_command(int sockd);

/**
 * Send ls command and wait for answer
 * @param sockd
 */
bool send_ls_command(int sockd, const char* path);

/**
 * Send mkdir command and wait for answer
 * @param sockd
 * @param path
 * @return
 */
bool send_mkdir_command(int sockd, const char* path);

/**
 * Send touch command and wait for answer
 * @param sockd
 * @param path
 * @return
 */
bool send_touch_command(int sockd, const char* path);

/**
 * Send open command and wait for answer
 * @param sockd
 * @param path
 * @return
 */
bool send_open_command(int sockd, const char* path);

/**
 * Send close command and wait for answer
 * @param sockd
 * @param path
 * @return
 */
bool send_close_command(int sockd, int fd);

/**
 * Send lseek command and wait for answer
 * @param sockd
 * @param size
 * @return
 */
bool send_lseek_command(int sockd, int fd, int size);

/**
 * Send write command and wait for answer
 * @param sockd
 * @param fd
 * @param data
 * @param size
 * @return
 */
bool send_write_command(int sockd, int fd, char* data, size_t size);

/**
 * Send read command and wait for answer
 * @param sockd
 * @param fd
 * @param size
 * @return
 */
bool send_read_command(int sockd, int fd, size_t size);

/**
 * Send write from command and wait for answer
 * @param sockd
 * @param fd
 * @param path
 * @return
 */
bool send_write_from_command(int sockd, int fd, const char* path);

/**
 * Send read to command and wait for answer
 * @param sockd
 * @param fd
 * @param path
 * @param size
 * @return
 */
bool send_read_to_command(int sockd, int fd, const char* path, size_t size);



#endif //EXT_CLIENT_HANDLERS_H_
