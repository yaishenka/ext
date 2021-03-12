/** @author yaishenka
    @date 11.03.2021 */
#ifndef EXT_FILESYSTEM_CORE_BLOCK_H_
#define EXT_FILESYSTEM_CORE_BLOCK_H_

#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include "inode.h"


/**
 * @brief Contains information about filename/dirname
 */
struct __attribute__((__packed__)) block_record {
  uint16_t inode_id;
  char* path;
};

/**
 * @brief Contains meta info about block
 */
struct __attribute__((__packed__)) block_info {
  uint16_t block_id;
  uint16_t inode_id;
  uint8_t records_count;
  uint16_t data_size;
};

/**
 * @brief Contains information about block
 *
 * Block can contain file data or records about directory
 */
struct __attribute__((__packed__)) block {
  struct block_info* block_info;
  struct block_record* block_records;
  char* data;
};

/**
 * @brief Constructor of block record
 * Init block record and set its path array to char[max_len_path]
 * @param block_record
 * @param superblock
 * @param inode_id id of inode with associated with dir or file
 */
void init_block_record(struct block_record* block_record,
                       const struct superblock* superblock,
                       uint16_t inode_id);

/**
 * @brief Destructor of block_record
 * @param block_record
 */
void destruct_block_record(struct block_record* block_record);

/**
 * @brief Constructor of block
 * Init block and set its block_records array to nullptr
 * @param block
 * @param superblock
 * @param block_id
 * @param inode_id
 */
void init_block(struct block* block,
                const struct superblock* superblock,
                uint16_t block_id,
                uint16_t inode_id);

/**
 * @brief Constructor of block
 * Init block and set its block_records array to bloc_record[records_count]
 * @param block
 * @param superblock
 * @param block_id
 * @param inode_id
 * @param records_count
 */
void init_block_with_records(struct block* block,
                             const struct superblock* superblock,
                             uint16_t block_id,
                             uint16_t inode_id,
                             uint8_t records_count);

/**
 * @brief Destructor of block
 * @param block
 */
void destruct_block(struct block* block);

/**
 * @brief Read block from memory
 * @param fd
 * @param block
 * @param block_id
 * @param superblock
 * @return sizeof(block) if reading is ok; -1 otherwise and destruct block object
 */
ssize_t read_block(int fd,
                   struct block* block,
                   uint16_t block_id,
                   const struct superblock* superblock);

/**
 * @brief Write
 * @param fd
 * @param block
 * @param superblock
 * @return sizeof(block) if writing is ok; -1 otherwise
 */
ssize_t write_block(int fd,
                    struct block* block,
                    const struct superblock* superblock);

/**
 * @param superblock
 * @return Maximum number of records in a block
 */
uint8_t get_max_records_count(const struct superblock* superblock);

/**
 * @param superblock
 * @return max data size in one block
 */
uint32_t get_max_data_in_block(const struct superblock* superblock);

/**
 * @param superblock
 * @return max data size in FS
 */
uint32_t get_max_data_size_of_all_blocks(const struct superblock* superblock);

/**
 * @param block
 * @param superblock
 * @return remain size of block
 */
uint32_t get_remain_data(const struct block* block, const struct superblock* superblock);

#endif //EXT_FILESYSTEM_CORE_BLOCK_H_
