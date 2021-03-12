/** @author yaishenka
    @date 11.03.2021 */
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "../utils.h"
#include "block.h"
#include "descriptors_table.h"

size_t max_size_of_data(const struct superblock* superblock) {
  return superblock->fs_info->block_size - sizeof(struct block_info);
}

size_t sizeof_block_record(const struct superblock* superblock) {
  return sizeof(uint16_t) + sizeof(char) * superblock->fs_info->max_path_len;
}

void init_block_info(struct block* block) {
  block->block_info = (struct block_info*) calloc(1, sizeof(struct block_info));
}

void init_block_record(struct block_record* block_record,
                       const struct superblock* superblock,
                       uint16_t inode_id) {
  block_record->inode_id = inode_id;
  block_record->path = (char*) calloc(superblock->fs_info->max_path_len,
                                      sizeof(char));
}

void destruct_block_record(struct block_record* block_record) {
  free(block_record->path);
}

ssize_t read_block_record(const int fd,
                          struct block_record* block_record,
                          const struct superblock* superblock) {
  block_record->path = (char*) calloc(superblock->fs_info->max_path_len,
                                      sizeof(char));

  uint16_t inode_id_array[1];
  ssize_t total_read = read_while(fd, (char*) inode_id_array, sizeof(uint16_t));

  if (total_read == -1) {
    fprintf(stderr, "%s", strerror(errno));
    destruct_block_record(block_record);
    return -1;
  }

  uint16_t inode_id = inode_id_array[0];

  block_record->inode_id = inode_id;

  ssize_t readed =
      read_while(fd, block_record->path, superblock->fs_info->max_path_len
          * sizeof(char));

  if (readed == -1) {
    fprintf(stderr, "%s", strerror(errno));
    destruct_block_record(block_record);
    return -1;
  }

  return readed + total_read;
}

ssize_t write_block_record(const int fd,
                           struct block_record* block_record,
                           const struct superblock* superblock) {
  ssize_t total_written =
      write_while(fd, (char*) &block_record->inode_id, sizeof(uint16_t));

  if (total_written == -1) {
    fprintf(stderr, "%s", strerror(errno));
    destruct_block_record(block_record);
    return -1;
  }

  ssize_t written = write_while(fd,
                                (char*) block_record->path,
                                superblock->fs_info->max_path_len
                                    * sizeof(char));

  if (written == -1) {
    fprintf(stderr, "%s", strerror(errno));
    destruct_block_record(block_record);
    return -1;
  }

  return written + total_written;
}

void init_block(struct block* block,
                const struct superblock* superblock,
                const uint16_t block_id,
                const uint16_t inode_id) {
  init_block_info(block);
  block->block_info->block_id = block_id;
  block->block_info->inode_id = inode_id;
  block->block_info->data_size = 0;
  block->block_info->records_count = 0;
  block->data = NULL;
  block->block_records = NULL;
}

void init_block_with_records(struct block* block,
                             const struct superblock* superblock,
                             const uint16_t block_id,
                             const uint16_t inode_id,
                             const uint8_t records_count) {
  init_block(block, superblock, block_id, inode_id);
  block->block_info->records_count = records_count;
  block->block_records = (struct block_record*) calloc(records_count,
                                                       sizeof(struct block_record));
}

void destruct_block(struct block* block) {
  if (block->data != NULL) {
    free(block->data);
  }

  for (uint8_t i = 0; i < block->block_info->records_count; ++i) {
    destruct_block_record(block->block_records + i);
  }

  free(block->block_info);
}

ssize_t read_block(const int fd,
                   struct block* block,
                   uint16_t block_id,
                   const struct superblock* superblock) {
  init_block_info(block);
  block->data = NULL;
  block->block_records = NULL;
  lseek(fd,
        sizeof_superblock(superblock) + sizeof_descriptors_table(superblock)
            + sizeof_inodes_block(superblock)
            + block_id * superblock->fs_info->block_size,
        SEEK_SET);

  ssize_t total_read =
      read_while(fd, (char*) block->block_info, sizeof(struct block_info));

  if (total_read == -1) {
    fprintf(stderr, "%s", strerror(errno));
    free(block->block_info);
    return -1;
  }

  if (block->block_info->records_count != 0
      && block->block_info->data_size != 0) {
    fprintf(stderr, "Block with data and records!");
    destruct_block(block);
    return -1;
  }

  if (block->block_info->records_count != 0) {
    block->block_records =
        (struct block_record*) calloc(block->block_info->records_count,
                                      sizeof(struct block_record));

    for (uint8_t i = 0; i < block->block_info->records_count; ++i) {
      ssize_t
          readed = read_block_record(fd, block->block_records + i, superblock);
      if (readed == -1) {
        fprintf(stderr, "%s", strerror(errno));
        destruct_block(block);
        return -1;
      }

      total_read += readed;
    }

    return total_read;
  } else if (block->block_info->data_size != 0) {
    block->data = (char*) calloc(max_size_of_data(superblock), sizeof(char));
    ssize_t readed = read_while(fd, block->data, block->block_info->data_size);
    total_read += readed;
  }

  return total_read;
}

ssize_t write_block(const int fd,
                    struct block* block,
                    const struct superblock* superblock) {
  lseek(fd,
        sizeof_superblock(superblock) + sizeof_descriptors_table(superblock)
            + sizeof_inodes_block(superblock)
            + block->block_info->block_id * superblock->fs_info->block_size,
        SEEK_SET);

  ssize_t total_written =
      write_while(fd, (char*) block->block_info, sizeof(struct block_info));

  if (total_written == -1) {
    fprintf(stderr, "%s", strerror(errno));
    return -1;
  }

  if (block->block_info->records_count != 0
      && block->block_info->data_size != 0) {
    fprintf(stderr, "Block with data and records!");
    return -1;
  }

  if (block->block_records != 0) {
    for (uint8_t i = 0; i < block->block_info->records_count; ++i) {
      ssize_t written =
          write_block_record(fd, block->block_records + i, superblock);

      if (written == -1) {
        fprintf(stderr, "%s", strerror(errno));
        destruct_block(block);
        return -1;
      }

      total_written += written;
    }
  } else if (block->block_info->data_size != 0) {
    ssize_t written = write_while(fd,
                                  block->data,
                                  max_size_of_data(superblock) * sizeof(char));

    if (written == -1) {
      fprintf(stderr, "%s", strerror(errno));
      return -1;
    }

    total_written += written;
  }

  return total_written;
}

uint8_t get_max_records_count(const struct superblock* superblock) {
  return (uint8_t) (superblock->fs_info->block_size - sizeof(struct block_info)) / sizeof_block_record(superblock);
}

