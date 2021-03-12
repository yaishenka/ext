/** @author yaishenka
    @date 11.03.2021 */
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "inode.h"
#include "../utils.h"
#include "descriptors_table.h"

size_t sizeof_inode(const struct superblock* superblock) {
  return sizeof(struct inode_info)
      + sizeof(uint16_t) * superblock->fs_info->blocks_count;
}

size_t calculate_offset(const struct superblock* superblock,
                        uint16_t inode_id) {
  return sizeof_superblock(superblock) + sizeof_descriptors_table(superblock)
      + inode_id * sizeof_inode(superblock);
}

void init_inode_arrays(struct inode* inode, const struct superblock* superblock) {
  inode->block_ids =
      (uint16_t*) calloc(superblock->fs_info->blocks_count, sizeof(uint16_t));
}

void init_inode_info(struct inode* inode) {
  inode->inode_info = (struct inode_info*) calloc(1, sizeof(struct inode_info));
}

void init_inode(struct inode* inode, uint16_t id, bool is_file, const struct superblock* superblock) {
  init_inode_info(inode);
  inode->inode_info->id = id;
  inode->inode_info->is_file = is_file;
  init_inode_arrays(inode, superblock);
}
void destroy_inode(struct inode* inode) {
  free(inode->inode_info);
  free(inode->block_ids);
}

ssize_t read_inode(int fd,
                   struct inode* inode,
                   uint16_t inode_id,
                   const struct superblock* superblock) {
  init_inode_info(inode);
  lseek(fd,
        calculate_offset(superblock, inode_id),
        SEEK_SET);

  ssize_t total_read =
      read_while(fd, (char*) inode->inode_info, sizeof(struct inode_info));

  if (total_read == -1) {
    fprintf(stderr, "%s", strerror(errno));
    free(inode->inode_info);
    return -1;
  }

  init_inode_arrays(inode, superblock);

  ssize_t readed = read_while(fd,
                              (char*) inode->block_ids,
                              sizeof(uint16_t) * superblock->fs_info->blocks_count);

  if (readed == -1) {
    fprintf(stderr, "%s", strerror(errno));
    destroy_inode(inode);
    return -1;
  }

  return total_read + readed;
}

ssize_t write_inode(int fd,
                    struct inode* inode,
                    const struct superblock* superblock) {
  lseek(fd,
        calculate_offset(superblock, inode->inode_info->id),
        SEEK_SET);

  ssize_t total_written = write_while(fd,
                                      (const char*) inode->inode_info,
                                      sizeof(struct inode_info));
  if (total_written == -1) {
    fprintf(stderr, "%s", strerror(errno));
    return -1;
  }

  ssize_t written = write_while(fd,
                                (const char*) inode->block_ids,
                                sizeof(uint16_t) * superblock->fs_info->blocks_count);

  if (written == -1) {
    fprintf(stderr, "%s", strerror(errno));
    return -1;
  }

  return total_written + written;
}

uint16_t sizeof_inodes_block(const struct superblock* superblock) {
  return superblock->fs_info->inodes_count * sizeof_inode(superblock);
}