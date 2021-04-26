cmake_minimum_required(VERSION 3.17)
project(ext C)

set(CMAKE_C_STANDARD 11)


add_subdirectory(client)
add_subdirectory(utils)
add_subdirectory(FileSystem)
#add_executable(ext main.c FileSystem/core/superblock.c FileSystem/core/defines.h FileSystem/core/inode.c FileSystem/core/block.c FileSystem/core/descriptors_table.c FileSystem/core/descriptors_table.h FileSystem/core/methods.h FileSystem/interface/init.h FileSystem/interface/ls.h FileSystem/core/methods.c FileSystem/interface/client.h FileSystem/interface/create_dir.h FileSystem/interface/create_file.h FileSystem/interface/open_file.h FileSystem/interface/open_file.h FileSystem/interface/close_file.h FileSystem/interface/write_to_file.h FileSystem/interface/read_file.h FileSystem/interface/lseek_pos.h)
target_include_directories(client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/utils)
target_include_directories(server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/utils)
target_link_libraries(server PRIVATE utils)
target_link_libraries(client PRIVATE utils)
