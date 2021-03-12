/** @author yaishenka
    @date 12.03.2021 */
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "../utils.h"
#include "descriptors_table.h"

void init_descriptors_table(struct descriptors_table* descriptors_table, const struct superblock* superblock) {
  descriptors_table->reserved_fd = (bool*) calloc(superblock->fs_info->descriptors_count, sizeof(bool));
  descriptors_table->fd_to_inode = (uint16_t*) calloc(superblock->fs_info->descriptors_count, sizeof(uint16_t));
  descriptors_table->fd_to_position = (uint32_t*) calloc(superblock->fs_info->descriptors_count, sizeof(uint32_t));
}

void destruct_descriptors_table(struct descriptors_table* descriptors_table, const struct superblock* superblock) {
  free(descriptors_table->reserved_fd);
  free(descriptors_table->fd_to_inode);
  free(descriptors_table->fd_to_position);
}

ssize_t read_descriptors_table(const int fd, struct descriptors_table* descriptors_table, const struct superblock* superblock) {
  init_descriptors_table(descriptors_table, superblock);
  lseek(fd, sizeof_superblock(superblock), SEEK_SET);
  uint16_t descriptors_count = superblock->fs_info->descriptors_count;
  ssize_t total_readed = read_while(fd, (char*) descriptors_table->reserved_fd, sizeof(bool) * descriptors_count);
  if (total_readed == -1) {
    fprintf(stderr, "%s", strerror(errno));
    destruct_descriptors_table(descriptors_table, superblock);
    return -1;
  }

  ssize_t readed = read_while(fd, (char*) descriptors_table->fd_to_inode, sizeof(uint16_t) * descriptors_count);
  if (readed == -1) {
    fprintf(stderr, "%s", strerror(errno));
    destruct_descriptors_table(descriptors_table, superblock);
    return -1;
  }
  total_readed += readed;

  readed = read_while(fd, (char*) descriptors_table->fd_to_position, sizeof(uint32_t) * descriptors_count);
  if (readed == -1) {
    fprintf(stderr, "%s", strerror(errno));
    destruct_descriptors_table(descriptors_table, superblock);
    return -1;
  }
  total_readed += readed;

  return total_readed;
}

ssize_t write_descriptor_table(const int fd, struct descriptors_table* descriptors_table, const struct superblock* superblock) {
  lseek(fd, sizeof_superblock(superblock), SEEK_SET);
  uint16_t descriptors_count = superblock->fs_info->descriptors_count;

  ssize_t total_written = write_while(fd, (char*) descriptors_table->reserved_fd, sizeof(bool) * descriptors_count);
  if (total_written == -1) {
    fprintf(stderr, "%s", strerror(errno));
    return -1;
  }

  ssize_t written = write_while(fd, (char*) descriptors_table->fd_to_inode, sizeof(uint16_t) * descriptors_count);
  if (written == -1) {
    fprintf(stderr, "%s", strerror(errno));
    return -1;
  }
  total_written += written;

  written = write_while(fd, (char*) descriptors_table->fd_to_position, sizeof(uint32_t) * descriptors_count);
  if (written == -1) {
    fprintf(stderr, "%s", strerror(errno));
    return -1;
  }
  total_written += written;

  return total_written;
}

uint16_t sizeof_descriptors_table(const struct superblock* superblock) {
  uint16_t descriptors_count = superblock->fs_info->descriptors_count;
  return descriptors_count * (sizeof(bool) + sizeof(uint16_t) + sizeof(uint32_t));
}
