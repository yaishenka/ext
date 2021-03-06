/**
 * @file defines.h
 * @author yaishenka
 * @date 13.03.2021
 * @brief Contains constants of FS
 */
#ifndef EXT_FILESYSTEM_CORE_DEFINES_H_
#define EXT_FILESYSTEM_CORE_DEFINES_H_

#define BLOCK_SIZE 128
#define INODES_COUNT 128
#define BLOCKS_COUNT 128
#define BLOCKS_COUNT_IN_INODE 8
#define MAX_PATH_LEN 16
#define DESCRIPTORS_COUNT 16
#define MAGIC 0xFAF
#define ROOT_INODE_ID 0
#define ROOT_BLOCK_ID 0

#endif //EXT_FILESYSTEM_CORE_DEFINES_H_
