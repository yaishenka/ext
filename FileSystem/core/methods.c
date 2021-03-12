/** @author yaishenka
    @date 12.03.2021 */

#include "methods.h"
#include "../utils.h"
#include "defines.h"

uint16_t create_dir_helper(const int fd, const struct superblock* superblock, uint16_t parent_node_id, bool is_root) {
  uint16_t new_inode_id = reserve_inode(superblock);
  if (new_inode_id == superblock->fs_info->inodes_count) {
    fprintf(stderr, "Can't create more inodes. Abort!\n");
    return superblock->fs_info->inodes_count;
  }

  uint16_t new_block_id = reserve_block(superblock);
  if (new_block_id == superblock->fs_info->blocks_count) {
    fprintf(stderr, "Can't create more blocks. Abort!\n");
    free_inode(superblock, new_inode_id);
    return superblock->fs_info->inodes_count;
  }

  if (is_root) {
    parent_node_id = new_inode_id;
  }

  struct inode inode;
  init_inode(&inode, new_inode_id, false, superblock);
  inode.block_ids[0] = new_block_id;

  struct block block;
  init_block_with_records(&block, superblock, new_block_id, new_inode_id, 2);

  struct block_record dot_record;
  init_block_record(&dot_record, superblock, new_inode_id);
  memcpy(dot_record.path, ".", 1);
  block.block_records[0] = dot_record;

  struct block_record two_dots_record;
  init_block_record(&two_dots_record, superblock, parent_node_id);
  memcpy(two_dots_record.path, "..", 2);
  block.block_records[1] = two_dots_record;

  if (write_block(fd, &block, superblock) == -1) {
    fprintf(stderr, "Can't write block. Abort!\n");
    free_inode(superblock, new_inode_id);
    free_block(superblock, new_block_id);
    destroy_inode(&inode);
    destruct_block(&block);
    return superblock->fs_info->inodes_count;
  }
  destruct_block(&block);

  if (write_inode(fd, &inode, superblock) == -1) {
    fprintf(stderr, "Can't write inode. Abort!\n");
    free_inode(superblock, new_inode_id);
    free_block(superblock, new_block_id);
    destroy_inode(&inode);
    return superblock->fs_info->inodes_count;
  }
  destroy_inode(&inode);

  if(write_super_block(fd, superblock) == -1) {
    fprintf(stderr, "Can't write superblock. Abort!\n");
    free_inode(superblock, new_inode_id);
    free_block(superblock, new_block_id);
    return superblock->fs_info->inodes_count;
  }

  return new_inode_id;
}

uint16_t create_file_helper(int fd, const struct superblock* superblock, uint16_t parent_node_id) {
  uint16_t new_inode_id = reserve_inode(superblock);
  if (new_inode_id == superblock->fs_info->inodes_count) {
    fprintf(stderr, "Can't create more inodes. Abort!\n");
    return superblock->fs_info->inodes_count;
  }

  uint16_t new_block_id = reserve_block(superblock);
  if (new_block_id == superblock->fs_info->blocks_count) {
    fprintf(stderr, "Can't create more blocks. Abort!\n");
    free_inode(superblock, new_inode_id);
    return superblock->fs_info->inodes_count;
  }

  struct inode inode;
  init_inode(&inode, new_inode_id, true, superblock);
  inode.block_ids[0] = new_block_id;

  struct block block;
  init_block(&block, superblock, new_block_id, new_inode_id);

  if (write_block(fd, &block, superblock) == -1) {
    fprintf(stderr, "Can't write block. Abort!\n");
    free_inode(superblock, new_inode_id);
    free_block(superblock, new_block_id);
    destroy_inode(&inode);
    destruct_block(&block);
    return superblock->fs_info->inodes_count;
  }
  destruct_block(&block);

  if (write_inode(fd, &inode, superblock) == -1) {
    fprintf(stderr, "Can't write inode. Abort!\n");
    free_inode(superblock, new_inode_id);
    free_block(superblock, new_block_id);
    destroy_inode(&inode);
    return superblock->fs_info->inodes_count;
  }
  destroy_inode(&inode);

  if(write_super_block(fd, superblock) == -1) {
    fprintf(stderr, "Can't write superblock. Abort!\n");
    free_inode(superblock, new_inode_id);
    free_block(superblock, new_block_id);
    return superblock->fs_info->inodes_count;
  }

  return new_inode_id;
}

bool get_inode_id_of_dir_rec(const int fd, const char* path, uint16_t* current_inode_id, const struct superblock* superblock) {
  struct inode inode;
  if (read_inode(fd, &inode, *current_inode_id, superblock) == -1) {
    fprintf(stderr, "Can't read inode. Abort!\n");
    return false;
  }

  if (inode.inode_info->is_file) {
    fprintf(stderr, "Trying to list file. Abort!\n");
    destroy_inode(&inode);
    return false;
  }

  char* current_file_name = (char*) calloc(superblock->fs_info->max_path_len, sizeof(char));
  char* path_to_parse = parse_path(path, current_file_name);

  if (strcmp(current_file_name, "/") == 0) {
    destroy_inode(&inode);
    free(current_file_name);
    free(path_to_parse);
    return true;
  }

  struct block block;
  read_block(fd, &block, inode.block_ids[0], superblock);

  bool founded = false;
  uint16_t right_record_id = 0;

  for (; right_record_id < block.block_info->records_count; ++right_record_id) {
    if (strcmp(block.block_records[right_record_id].path, current_file_name) == 0) {
      founded = true;
      break;
    }
  }

  if (!founded) {
    fprintf(stderr, "Directory doesn't exist. Abort!\n");
    destruct_block(&block);
    destroy_inode(&inode);
    free(current_file_name);
    free(path_to_parse);
    return false;
  }

  *current_inode_id = block.block_records[right_record_id].inode_id;

  if (path_to_parse == NULL) {
    destruct_block(&block);
    destroy_inode(&inode);
    free(current_file_name);
    free(path_to_parse);
    return true;
  } else {
    bool result = get_inode_id_of_dir_rec(fd, path_to_parse, current_inode_id, superblock);
    destruct_block(&block);
    destroy_inode(&inode);
    free(current_file_name);
    free(path_to_parse);
    return result;
  }

}

uint16_t get_inode_id_of_dir(const int fd, const char* path, const struct superblock* superblock) {
  uint16_t current_inode_id = ROOT_INODE_ID;

  if (!get_inode_id_of_dir_rec(fd, path, &current_inode_id, superblock)) {
    fprintf(stderr, "Can't find inode. Abort!\n");
    return superblock->fs_info->inodes_count;
  }

  return current_inode_id;
}

bool is_dir_exist(const int fd, struct inode* inode, const char* dirname, const struct superblock* superblock) {
  struct block block;
  if (read_block(fd, &block, inode->block_ids[0], superblock) == -1) {
    fprintf(stderr, "Can't read block. Abort without cleaning!\n");
    exit(EXIT_FAILURE);
  }

  for (uint16_t record_id = 0; record_id < block.block_info->records_count; ++record_id) {
    if (strcmp(dirname, block.block_records[record_id].path) == 0) {
      destruct_block(&block);
      return true;
    }
  }

  destruct_block(&block);
  return false;
}