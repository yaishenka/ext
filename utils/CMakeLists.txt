set(LIBRARY_NAME utils)
option(BUILD_SHARED_LIBS True)

add_library(${LIBRARY_NAME} commands.h utils.h utils.c net_utils.h net_utils.c)