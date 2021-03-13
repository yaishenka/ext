/**
 * @file descriptors_table.h
 * @author yaishenka
 * @date 13.03.2021
 * @brief Contains descriptors_table struct and its methods
 */
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
void init_descriptors_table(struct descriptors_table* descriptors_table,
                            const struct superblock* superblock);

/**
 * @brief Destructor of superblock
 * @param descriptors_table
 * @param superblock
 */
void destruct_descriptors_table(struct descriptors_table* descriptors_table,
                                const struct superblock* superblock);

/**
 * @brief Read descriptors_table from memory
 * @param fd opened fd
 * @param descriptors_table
 * @param superblock
 * @return sizeof(descriptor_table) of reading is ok; -1 otherwise and destruct descriptors_table
 */
ssize_t read_descriptors_table(int fd,
                               struct descriptors_table* descriptors_table,
                               const struct superblock* superblock);

/**
 * @brief Write descriptors_table to memory
 * @param fd opened fd
 * @param descriptors_table
 * @param superblock
 * @return sizeof(descriptor_table) of writing is ok; -1 otherwise
 */
ssize_t write_descriptor_table(int fd,
                               struct descriptors_table* descriptors_table,
                               const struct superblock* superblock);

/**
 * @brief Occupy descriptor for inode_id
 * @param descriptors_table
 * @param inode_id
 * @param superblock
 * @return fd if all ok; -1 otherwise
 */
int reserve_descriptor(struct descriptors_table* descriptors_table,
                       uint16_t inode_id,
                       const struct superblock* superblock);

/**
 * @brief Release descriptor for inode_id
 * @param descriptors_table
 * @param fd fd to release
 * @param superblock
 * @return d if all ok; -1 otherwise
 */
int free_descriptor(struct descriptors_table* descriptors_table,
                    uint16_t fd,
                    const struct superblock* superblock);

/**
 * @brief Sizeof descriptors_table
 * @param superblock
 * @return
 */
uint16_t sizeof_descriptors_table(const struct superblock* superblock);

#endif //EXT_FILESYSTEM_CORE_DESCRIPTORS_TABLE_H_
