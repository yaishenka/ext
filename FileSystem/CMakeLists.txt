set(
        SERVER_SOURCES
        core/block.c
        core/descriptors_table.c
        core/inode.c
        core/methods.c
        core/superblock.c
        server/server.c
        main.c
)

add_executable(
        server
        ${SERVER_SOURCES}
)

add_executable(client_without_server
        core/block.c
        core/descriptors_table.c
        core/inode.c
        core/methods.c
        core/superblock.c
        interface/main.c
)

target_include_directories(server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/interface)
target_include_directories(client_without_server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/interface)