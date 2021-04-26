set(
        CLIENT_SOURCES
        main.c
        client.c
        handlers.c
)

add_executable(
        client
        ${CLIENT_SOURCES}
)