/** @author yaishenka
    @date 12.03.2021 */
#ifndef EXT_FILESYSTEM_CORE_METHODS_H_
#define EXT_FILESYSTEM_CORE_METHODS_H_
#include <string.h>
#include <stdio.h>
#include "superblock.h"
#include "block.h"
#include "inode.h"

/**
 * @brief Helper for create new directory
 * Creates dir with parent = parent_node_id (or itself if is_root).
 * Push to new dir records about "." and ".."
 * @param fd opened fd
 * @param superblock
 * @param parent_node_id parent of new dir
 * @param is_root
 * @return id of new inode if all ok; superblock->fs_info->inodes_count otherwise
 */
uint16_t create_dir_helper(int fd,
                           const struct superblock* superblock,
                           uint16_t parent_node_id,
                           bool is_root);

/**
 * @brief Helper for create new file
 * Creates file with parent = parent_node_id
 * @param fd
 * @param superblock
 * @param parent_node_id
 * @return id of new inode if all ok; superblock->fs_info->inodes_count otherwise
 */
uint16_t create_file_helper(int fd,
                            const struct superblock* superblock,
                            uint16_t parent_node_id);

/**
 * @brief Parse path and find inode of this dir
 * @param fd
 * @param path
 * @param superblock
 * @return id of inode of this dir if all ok; superblock->fs_info.inodes_count otherwise
 */
uint16_t get_inode_id_of_dir(int fd,
                             const char* path,
                             const struct superblock* superblock);

/**
 * @brief Check if dir exists in inode
 * @param fd
 * @param inode
 * @param dirname
 * @param superblock
 * @return
 */
bool is_dir_exist(int fd,
                  struct inode* inode,
                  const char* dirname,
                  const struct superblock* superblock);

/**
 * @brief Return inode_id of file
 * @param fd
 * @param inode
 * @param dirname
 * @param superblock
 * @return inode_id of file if it exists in this node; superblock->fs_info.inodes_count otherwise
 */
uint16_t get_file_inode_id(int fd,
                           struct inode* inode,
                           const char* dirname,
                           const struct superblock* superblock);

#endif //EXT_FILESYSTEM_CORE_METHODS_H_
