/** @author yaishenka
    @date 12.03.2021 */
#ifndef EXT_FILESYSTEM_INTERFACE_WRITE_TO_FILE_H_
#define EXT_FILESYSTEM_INTERFACE_WRITE_TO_FILE_H_

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include "../core/superblock.h"
#include "../core/descriptors_table.h"
#include "../core/defines.h"
#include "../core/methods.h"
#include "../utils.h"

/**
 * @brief Write data to file
 * Write data from data to file by file_descriptor.
 * @param fs_file_path
 * @param file_descriptor opened file descriptor from our FS
 * @param data data to write
 * @param size size should be \leq max_data_size
 */
void write_to_file(const char* path_to_fs_file,
                   uint16_t file_descriptor,
                   char* data,
                   uint32_t size) {
  int fd = open(path_to_fs_file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    fprintf(stderr, "Can't open file. Abort!\n");
    exit(EXIT_FAILURE);
  }

  struct superblock superblock;
  read_super_block(fd, &superblock);
  if (superblock.fs_info->magic != MAGIC) {
    fprintf(stderr, "Magic does not match. Abort!\n");
    destroy_super_block(&superblock);
    close(fd);
    exit(EXIT_FAILURE);
  }

  if (!superblock.reserved_inodes_mask[ROOT_INODE_ID]) {
    fprintf(stderr, "Root directory doesn't exist. Abort!\n");
    destroy_super_block(&superblock);
    close(fd);
    exit(EXIT_FAILURE);
  }

  if (size > get_max_data_size_of_all_blocks(&superblock)) {
    size = get_max_data_size_of_all_blocks(&superblock);
  }

  struct descriptors_table descriptors_table;
  if (read_descriptors_table(fd, &descriptors_table, &superblock) == -1) {
    fprintf(stderr, "Can't read descriptors_table. Abort!\n");
    destroy_super_block(&superblock);
    close(fd);
    exit(EXIT_FAILURE);
  }

  if (!descriptors_table.reserved_fd[file_descriptor]) {
    fprintf(stderr, "Trying to write to closed fd. Abort!\n");
    destruct_descriptors_table(&descriptors_table, &superblock);
    destroy_super_block(&superblock);
    close(fd);
    return;
  }

  uint32_t fd_position = descriptors_table.fd_to_position[file_descriptor];
  uint16_t inode_id = descriptors_table.fd_to_inode[file_descriptor];

  struct inode inode;
  if (read_inode(fd, &inode, inode_id, &superblock) == -1) {
    fprintf(stderr, "Can't read inode. Abort!\n");
    destruct_descriptors_table(&descriptors_table, &superblock);
    destroy_super_block(&superblock);
    close(fd);
    exit(EXIT_FAILURE);
  }

  uint32_t total_written = 0;
  uint32_t need_to_write_size = size;
  while (total_written != size) {
    uint16_t block_to_write_pos =
        (uint16_t) fd_position / get_max_data_in_block(&superblock);
    struct block block;
    if (block_to_write_pos < inode.inode_info->blocks_count) {
      if (read_block(fd,
                     &block,
                     inode.block_ids[block_to_write_pos],
                     &superblock) == -1) {
        fprintf(stderr, "Can't read block. Abort!\n");
        destruct_descriptors_table(&descriptors_table, &superblock);
        destroy_inode(&inode);
        destroy_super_block(&superblock);
        close(fd);
        exit(EXIT_FAILURE);
      }
    } else {
      uint16_t new_block_id = reserve_block(&superblock);
      if (new_block_id == superblock.fs_info->blocks_count) {
        fprintf(stderr, "Can't create more blocks in FS. Abort!\n");

        descriptors_table.fd_to_position[file_descriptor] = fd_position;
        if (write_descriptor_table(fd, &descriptors_table, &superblock) == -1) {
          fprintf(stderr, "Can't write descriptor table. Abort!\n");
          destruct_descriptors_table(&descriptors_table, &superblock);
          destroy_inode(&inode);
          destroy_super_block(&superblock);
          close(fd);
          exit(EXIT_FAILURE);
        }
        destruct_descriptors_table(&descriptors_table, &superblock);

        if (write_inode(fd, &inode, &superblock) == -1) {
          fprintf(stderr, "Can't write inode. Abort!\n");
          destroy_inode(&inode);
          destroy_super_block(&superblock);
          close(fd);
          exit(EXIT_FAILURE);
        }
        destroy_inode(&inode);

        if (write_super_block(fd, &superblock) == -1) {
          fprintf(stderr, "Can't write superblock. Abort!\n");
          destroy_super_block(&superblock);
          close(fd);
          exit(EXIT_FAILURE);
        }
        destroy_super_block(&superblock);
        close(fd);
        return;
      }

      if (inode.inode_info->blocks_count
          == superblock.fs_info->blocks_count_in_inode) {
        fprintf(stderr, "Can't create more blocks in this inode. Abort!\n");

        descriptors_table.fd_to_position[file_descriptor] = fd_position;
        if (write_descriptor_table(fd, &descriptors_table, &superblock) == -1) {
          fprintf(stderr, "Can't write descriptor table. Abort!\n");
          destruct_descriptors_table(&descriptors_table, &superblock);
          destroy_inode(&inode);
          destroy_super_block(&superblock);
          close(fd);
          exit(EXIT_FAILURE);
        }
        destruct_descriptors_table(&descriptors_table, &superblock);

        if (write_inode(fd, &inode, &superblock) == -1) {
          fprintf(stderr, "Can't write inode. Abort!\n");
          destroy_inode(&inode);
          destroy_super_block(&superblock);
          close(fd);
          exit(EXIT_FAILURE);
        }
        destroy_inode(&inode);

        if (write_super_block(fd, &superblock) == -1) {
          fprintf(stderr, "Can't write superblock. Abort!\n");
          destroy_super_block(&superblock);
          close(fd);
          exit(EXIT_FAILURE);
        }
        destroy_super_block(&superblock);
        close(fd);
        return;
      }
      init_block(&block, &superblock, new_block_id, inode_id);
      block.data = (char*) calloc(superblock.fs_info->block_size, sizeof(char));
      inode.block_ids[inode.inode_info->blocks_count] = new_block_id;
      inode.inode_info->blocks_count += 1;
      if (write_inode(fd, &inode, &superblock) == -1) {
        fprintf(stderr, "Can't write inode. Abort!\n");
        destruct_block(&block);
        destroy_inode(&inode);
        destroy_super_block(&superblock);
        close(fd);
        exit(EXIT_FAILURE);
      }
    }

    uint32_t position_in_block_data =
        (uint32_t) fd_position % get_max_data_in_block(&superblock);
    char* position_to_write = block.data + position_in_block_data;
    uint32_t remain_size =
        get_max_data_in_block(&superblock) - position_in_block_data;
    uint32_t size_to_write = need_to_write_size < remain_size ? need_to_write_size : remain_size;
    memcpy(position_to_write, data, size_to_write);
    block.block_info->data_size += size_to_write;
    if (write_block(fd, &block, &superblock) == -1) {
      fprintf(stderr, "Can't write block. Abort!\n");
      destruct_block(&block);
      destroy_inode(&inode);
      destroy_super_block(&superblock);
      close(fd);
      exit(EXIT_FAILURE);
    }
    destruct_block(&block);

    fd_position += size_to_write;
    data += size_to_write;
    total_written += size_to_write;
    need_to_write_size -= size_to_write;
  }

  descriptors_table.fd_to_position[file_descriptor] = fd_position;
  if (write_descriptor_table(fd, &descriptors_table, &superblock) == -1) {
    fprintf(stderr, "Can't write descriptor table. Abort!\n");
    destruct_descriptors_table(&descriptors_table, &superblock);
    destroy_inode(&inode);
    destroy_super_block(&superblock);
    close(fd);
    exit(EXIT_FAILURE);
  }
  destruct_descriptors_table(&descriptors_table, &superblock);

  if (write_inode(fd, &inode, &superblock) == -1) {
    fprintf(stderr, "Can't write inode. Abort!\n");
    destroy_inode(&inode);
    destroy_super_block(&superblock);
    close(fd);
    exit(EXIT_FAILURE);
  }
  destroy_inode(&inode);

  if (write_super_block(fd, &superblock) == -1) {
    fprintf(stderr, "Can't write superblock. Abort!\n");
    destroy_super_block(&superblock);
    close(fd);
    exit(EXIT_FAILURE);
  }
  destroy_super_block(&superblock);
  close(fd);

  printf("Total written: %d\n", total_written);
}

#endif //EXT_FILESYSTEM_INTERFACE_WRITE_TO_FILE_H_
