/**
 * @file net_utils.h
 * @author yaishenka
 * @date 25.04.2021
 */
#ifndef EXT_UTILS_NET_UTILS_H_
#define EXT_UTILS_NET_UTILS_H_
#include "commands.h"
#include "utils.h"

/**
 * Split address to port and host
 * @param address
 * @param host
 * @param port
 */
void parse_address(const char* address, char** host, long* port);

/**
 * Establish connection with address
 * @param address
 * @return socket
 */
int make_connection(const char* address);

/**
 * Close connection
 * @param sockd
 */
void close_connection(int sockd);

/**
 * Send command from Command enum to socket
 * @param sockd
 * @param command
 */
bool send_command(int sockd, enum Command command);

/**
 * Send data to socket
 * @param sockd
 * @param data
 * @param size
 */
bool send_data(int sockd, char* data, size_t size);

/**
 * Receive command from fd
 * @param fd
 * @param command
 * @return true if all ok and false otherwise
 */
bool receive_command(int sockd, enum Command* command);


/**
 * Receive data from socket. Write data to data and size to size
 * @param sockd
 * @param data
 * @param size
 * @return true if all ok and false otherwise
 */
bool receive_data(int sockd, char** data, size_t* size);


#endif //EXT_UTILS_NET_UTILS_H_
