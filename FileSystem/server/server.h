/**
 * @file server.h
 * @author yaishenka
 * @date 26.04.2021
 */
#ifndef EXT_FILESYSTEM_SERVER_SERVER_H_
#define EXT_FILESYSTEM_SERVER_SERVER_H_

/**
 * Main loop for server
 * @param port
 * @param fs_file_path
 * @return
 */
int server_loop(long port, const char* fs_file_path);

#endif //EXT_FILESYSTEM_SERVER_SERVER_H_
