/**
 * @file handlers.h
 * @author yaishenka
 * @date 25.04.2021
 * @brief Contains handlers for commands from server
 */
#ifndef EXT_CLIENT_HANDLERS_H_
#define EXT_CLIENT_HANDLERS_H_

#include <stdbool.h>

/**
 * Read count of lines and lines from socket
 * Prixnt result to stdout
 * @param sockd
 */
bool receive_result(int sockd);

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

//bool send_touch_command(int sockd, const char* path);


#endif //EXT_CLIENT_HANDLERS_H_
