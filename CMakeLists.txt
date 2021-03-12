cmake_minimum_required(VERSION 3.16)
project(ext C)

set(CMAKE_C_STANDARD 11)

add_executable(ext main.c FileSystem/core/superblock.c FileSystem/core/defines.h FileSystem/utils.c  FileSystem/core/inode.c FileSystem/core/block.c FileSystem/core/descriptors_table.c FileSystem/core/descriptors_table.h FileSystem/core/methods.h FileSystem/interface/init.h FileSystem/interface/ls.h FileSystem/core/methods.c FileSystem/interface/client.h FileSystem/interface/create_dir.h FileSystem/interface/create_file.h)