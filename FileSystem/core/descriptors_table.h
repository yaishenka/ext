/** @author yaishenka
    @date 12.03.2021 */
#ifndef EXT_FILESYSTEM_CORE_DESCRIPTORS_TABLE_H_
#define EXT_FILESYSTEM_CORE_DESCRIPTORS_TABLE_H_
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include "superblock.h"

/**
 * @brief Struct for represent DT
 * Contains all information about descriptor table
 */
struct __attribute__((__packed__)) descriptors_table {
  bool* reserved_fd;
  uint16_t* fd_to_inode;
  uint32_t* fd_to_position;
};

/**
 * @brief Constructor of descriptors_table
 * Init descriptors table with metadata from superblock
 * @param descriptors_table
 * @param superblock
 */
void init_descriptors_table(struct descriptors_table* descriptors_table, const struct superblock* superblock);

/**
 * @brief Destructor of superblock
 * @param descriptors_table
 * @param superblock
 */
void destruct_descriptors_table(struct descriptors_table* descriptors_table, const struct superblock* superblock);

/**
 * @brief Read descriptors_table from memory
 * @param fd opened fd
 * @param descriptors_table
 * @param superblock
 * @return sizeof(descriptor_table) of reading is ok; -1 otherwise and destruct descriptors_table
 */
ssize_t read_descriptors_table(int fd, struct descriptors_table* descriptors_table, const struct superblock* superblock);

/**
 * @brief Write descriptors_table to memory
 * @param fd opened fd
 * @param descriptors_table
 * @param superblock
 * @return sizeof(descriptor_table) of writing is ok; -1 otherwise
 */
ssize_t write_descriptor_table(int fd, struct descriptors_table* descriptors_table, const struct superblock* superblock);

/**
 * @brief Sizeof descriptors_table
 * @param superblock
 * @return
 */
uint16_t sizeof_descriptors_table(const struct superblock* superblock);

#endif //EXT_FILESYSTEM_CORE_DESCRIPTORS_TABLE_H_
