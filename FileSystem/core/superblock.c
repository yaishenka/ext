/** @author yaishenka
    @date 10.03.2021 */
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "superblock.h"
#include "defines.h"
#include "../../utils/utils.h"

void init_superblock_arrays(struct superblock* superblock) {
  superblock->reserved_blocks_mask =
      (bool*) calloc(superblock->fs_info->blocks_count, sizeof(bool));
  superblock->reserved_inodes_mask =
      (bool*) calloc(superblock->fs_info->inodes_count, sizeof(bool));
}

void init_superblock_fs_info(struct superblock* superblock) {
  superblock->fs_info = (struct fs_info*) calloc(1, sizeof(struct fs_info));
}

size_t sizeof_superblock(const struct superblock* superblock) {
  return sizeof(struct fs_info)
      + sizeof(bool) * superblock->fs_info->inodes_count
      + sizeof(bool) * superblock->fs_info->blocks_count;
}

void init_super_block(struct superblock* superblock) {
  init_superblock_fs_info(superblock);
  superblock->fs_info->blocks_count_in_inode = BLOCKS_COUNT_IN_INODE;
  superblock->fs_info->blocks_count = BLOCKS_COUNT;
  superblock->fs_info->inodes_count = INODES_COUNT;
  superblock->fs_info->block_size = BLOCK_SIZE;
  superblock->fs_info->max_path_len = MAX_PATH_LEN;
  superblock->fs_info->descriptors_count = DESCRIPTORS_COUNT;
  superblock->fs_info->magic = MAGIC;
  init_superblock_arrays(superblock);
}

void destroy_super_block(struct superblock* superblock) {
  free(superblock->fs_info);
  free(superblock->reserved_blocks_mask);
  free(superblock->reserved_inodes_mask);
}

ssize_t read_super_block(const int fd, struct superblock* superblock) {
  init_superblock_fs_info(superblock);
  lseek(fd, 0, SEEK_SET);
  ssize_t total_read =
      read_while(fd, (char*) superblock->fs_info, sizeof(struct fs_info));

  if (total_read == -1) {
    fprintf(stderr, "%s", strerror(errno));
    free(superblock->fs_info);
    return -1;
  }

  init_superblock_arrays(superblock);
  ssize_t readed = read_while(fd,
                              (char*) superblock->reserved_inodes_mask,
                              superblock->fs_info->inodes_count);
  if (readed == -1) {
    fprintf(stderr, "%s", strerror(errno));
    destroy_super_block(superblock);
    return -1;
  }

  total_read += readed;

  readed = read_while(fd,
                      (char*) superblock->reserved_blocks_mask,
                      superblock->fs_info->blocks_count);
  if (readed == -1) {
    fprintf(stderr, "%s", strerror(errno));
    destroy_super_block(superblock);
    return -1;
  }

  total_read += readed;

  return total_read;
}

ssize_t write_super_block(const int fd, struct superblock* superblock) {
  lseek(fd, 0, SEEK_SET);
  ssize_t total_written = write_while(fd,
                                      (const char*) superblock->fs_info,
                                      sizeof(struct fs_info));

  if (total_written == -1) {
    fprintf(stderr, "%s", strerror(errno));
    return -1;
  }

  ssize_t written = write_while(fd,
                                (const char*) superblock->reserved_inodes_mask,
                                superblock->fs_info->inodes_count);
  if (written == -1) {
    fprintf(stderr, "%s\n", strerror(errno));
    return -1;
  }
  total_written += written;

  written = write_while(fd,
                        (const char*) superblock->reserved_blocks_mask,
                        superblock->fs_info->blocks_count);
  if (written == -1) {
    fprintf(stderr, "%s\n", strerror(errno));
    return -1;
  }
  total_written += written;

  return total_written;
}

uint16_t reserve_inode(const struct superblock* superblock) {
  for (uint16_t id = 0; id < superblock->fs_info->inodes_count; ++id) {
    if (!superblock->reserved_inodes_mask[id]) {
      superblock->reserved_inodes_mask[id] = true;
      return id;
    }
  }

  return superblock->fs_info->inodes_count;
}

uint16_t free_inode(const struct superblock* superblock,
                    const uint16_t inode_id) {
  if (superblock->reserved_inodes_mask[inode_id]) {
    superblock->reserved_inodes_mask[inode_id] = false;
    return inode_id;
  }

  return superblock->fs_info->inodes_count;
}

uint16_t reserve_block(const struct superblock* superblock) {
  for (uint16_t id = 0; id < superblock->fs_info->blocks_count; ++id) {
    if (!superblock->reserved_blocks_mask[id]) {
      superblock->reserved_blocks_mask[id] = true;
      return id;
    }
  }

  return superblock->fs_info->blocks_count;
}

uint16_t free_block(const struct superblock* superblock, uint16_t block_id) {
  if (superblock->reserved_blocks_mask[block_id]) {
    superblock->reserved_blocks_mask[block_id] = false;
    return block_id;
  }

  return superblock->fs_info->blocks_count;
}