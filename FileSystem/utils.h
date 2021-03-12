/** @author yaishenka
    @date 10.03.2021 */
#ifndef EXT_FILESYSTEM_UTILS_H_
#define EXT_FILESYSTEM_UTILS_H_
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>

#define buffer_length 1024


/**
 * @brief Properly reading from memory
 * @param fd
 * @param buffer
 * @param to_read
 * @return
 */
int read_while(int fd, char* buffer, size_t to_read);

/**
 * @brief Properly writng to memory
 * @param fd
 * @param buffer
 * @param to_write
 * @return
 */
int write_while(int fd, const char* buffer, size_t to_write);

/**
 * @brief Parse path
 * @param path
 * @param current_file_name
 * @return Position of second slash
 * @example Let path be /test/test2. Function will return position of second slash and write to current_file_name="test"
 * @warning Function can fail if path is incorrect (without / in begin). Will crash program
 */
char* parse_path(const char* path, char* current_file_name);

/**
 * @brief Split path to parent path and name
 * @param path
 * @param parent_path
 * @param name
 * @return true if all ok and false otherwise
 * @example Let path be /test/a. Function will save /test/ to parent_path and a to dirname
 */
bool split_path(const char* path, char* parent_path, char* dirname);

/**
 * @brief Read line from stdin
 * @param command_buffer
 * @param buffer_size
 */
void read_command_from_stdin(char* command_buffer, size_t buffer_size);

/**
 * @brief Parse command
 * Parse command to command and args.
 * Put command to arg command and return position of first arg
 * @param command_buffer
 * @param command
 * @return return position of first arg (first symbol after space)
 */
char* parse_command(char* command_buffer, char* command);

/**
 * @brief Get size of file
 * @param path_to_file
 * @return size of file if all ok; -1 otherwise;
 */
ssize_t get_file_size(const char* path_to_file);



#endif //EXT_FILESYSTEM_UTILS_H_
