/** @author yaishenka
    @date 10.03.2021 */
#ifndef EXT_FILESYSTEM_SUBERBLOCK_H_
#define EXT_FILESYSTEM_SUBERBLOCK_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Contains main information about FS
 */
struct __attribute__((__packed__)) fs_info {
  uint16_t inodes_count;
  uint16_t block_size;
  uint16_t blocks_count;
  uint16_t max_path_len;
  uint16_t descriptors_count;
  uint16_t magic;
};

/**
 * @brief Main suberblock struct
 * Contains fs_info and masks for blocks and inodes
 */
struct __attribute__((__packed__)) superblock {
  struct fs_info* fs_info;
  bool* reserved_inodes_mask;
  bool* reserved_blocks_mask;
};

/**
 * Count blocks_count of inode in bytes
 * @param superblock
 * @return blocks_count of superblock in FS
 */
size_t sizeof_superblock(const struct superblock* superblock);

/**
 * @brief Constructor of superblock
 *
 * Construct superblock with default params defined in core/defines.h
 * @param superblock
 */
void init_super_block(struct superblock* superblock);

/**
 * @brief Destructor of superblock
 * @param superblock
 */
void destroy_super_block(struct superblock* superblock);

/**
 * @brief Read sb from memory
 * @param fd opened fd
 * @param superblock empty instance of superblock
 * @return sizeof(superblock) if reading is ok; -1 otherwise and destruct superblock object
 * @warning printf strerror(errno) to stderr
 */
ssize_t read_super_block(int fd, struct superblock* superblock);

/**
 * @brief Write sb to memory
 * @param fd opened fd
 * @param superblock
 * @return sizeof(superblock) if writing is ok; -1 otherwise
 * @warning printf strerror(errno) to stderr
 */
ssize_t write_super_block(int fd, struct superblock* superblock);

/**
 * @brief Reserve free inode
 * @param superblock
 * @return Id of first free inode if it exists; superblock->fs_info->inodes_count if all inodes are reserved
 */
uint16_t reserve_inode(const struct superblock* superblock);

/**
 * @brief Release inode
 * @param superblock
 * @param inode_id id of inode to release
 * @return id of inode if all ok; superblock->fs_info->blocks_count if inode was already released
 */
uint16_t free_inode(const struct superblock* superblock, uint16_t inode_id);

/**
 * @brief Reserve block
 * @param superblock
 * @return block_id if all ok; superblock->fs_info->blocks_count if all block are reserved
 */
uint16_t reserve_block(const struct superblock* superblock);

/**
 * @brief Release block
 * @param superblock
 * @param block_id id of block to release
 * @return id of block if all ok; -1 if block was already released
 */
uint16_t free_block(const struct superblock* superblock, uint16_t block_id);





#endif //EXT_FILESYSTEM_SUBERBLOCK_H_