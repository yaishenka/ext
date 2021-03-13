/**
 * @file inode.h
 * @author yaishenka
 * @date 13.03.2021
 * @brief Contains inode struct and its methods
 */
#ifndef EXT_FILESYSTEM_INODE_H_
#define EXT_FILESYSTEM_INODE_H_

#include <stdint.h>
#include <stdbool.h>
#include "superblock.h"

/**
 * @brief Contains information about inode
 *
 * This struct contains info that can be simply written to memory
 */
struct __attribute__((__packed__)) inode_info {
  uint16_t id;
  uint16_t blocks_count;
  bool is_file;
};

/**
 * @brief Main inode struct
 *
 * This struct represent inode
 */
struct __attribute__((__packed__)) inode {
  struct inode_info* inode_info;
  uint16_t* block_ids;
};

/**
 * @brief Count size of inode in bytes
 * @param superblock the superblock with metadata of FS
 * @return size of inode in FS
 */
size_t sizeof_inode(const struct superblock* superblock);

/**
 * @brief Constructor of inode
 * @param inode empty instance of inode
 * @param id id of inode
 * @param is_file
 */
void init_inode(struct inode* inode,
                uint16_t id,
                bool is_file,
                const struct superblock* superblock);

/**
 * @brief Destructor of inode
 * @param inode
 */
void destroy_inode(struct inode* inode);

/**
 * @brief Read inode from memory
 * @param fd opened fd
 * @param inode empty instance of inode
 * @param inode_id id of inode to read
 * @param superblock the superblock with metadata of FS
 * @return sizeof(inode) if reading is ok; -1 otherwise and destruct inode object
 * @warning printf strerror(errno) to stderr
 */
ssize_t read_inode(int fd,
                   struct inode* inode,
                   uint16_t inode_id,
                   const struct superblock* superblock);

/**
 * @brief Write inode from memory
 * @param fd opened fd
 * @param inode instance of inode
 * @param superblock the superblock with metadata of FS
 * @return sizeof(inode) if writing is ok; -1 otherwise
 * @warning printf strerror(errno) to stderr
 */
ssize_t write_inode(int fd,
                    struct inode* inode,
                    const struct superblock* superblock);

/**
 * @brief Calculate size of block of all inodes
 * @param superblock
 * @return
 */
uint16_t sizeof_inodes_block(const struct superblock* superblock);

#endif //EXT_FILESYSTEM_INODE_H_
