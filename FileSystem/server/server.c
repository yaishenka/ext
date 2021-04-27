/** @author yaishenka
    @date 26.04.2021 */
#include "server.h"
#include "utils.h"
#include "net_utils.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#if __APPLE__
  #include <sys/event.h>
#else
  #include <sys/epoll.h>
#endif

#include "init.h"
#include "ls.h"
#include "create_dir.h"
#include "create_file.h"
#include "open_file.h"
#include "close_file.h"
#include "write_to_file.h"
#include "read_file.h"
#include "lseek_pos.h"


bool process_command(int sockd, const char* fs_file_path) {
  enum Command command;
  if (!receive_command(sockd, &command)) {
    return false;
  }

  switch (command) {
    case Help: {
      char* help_text = "You are working with minifs\n"
                        "Authored by yaishenka\n"
                        "Source available ad github.com/yaishenka/ext\n"
                        "Available commands:\n"
                        "help -- print this text\n"
                        "quit -- close program\n"
                        "ls [path] -- list directory contents\n"
                        "init -- init file system\n"
                        "read_fs -- read fs_file and checks it\n"
                        "mkdir [path] -- make directories\n"
                        "touch [path] -- create files\n"
                        "open [path] -- open file and return FD\n"
                        "close [fd] -- close FD\n"
                        "write [fd] [data] -- write data to FD\n"
                        "write_from [fd] [path] -- read data from path and write to FD\n"
                        "read [fd] [size] -- read size bytes from FD\n"
                        "read_to [fd] [path] [size] -- read file from fd.pos and write data to path. "
                        "If size not specified file will be readed till end\n"
                        "lseek [fd] [pos] -- set fd.pos = pos\n";
      send_data(sockd, help_text, strlen(help_text));
      break;
    }

    case Quit: {
      return false;
    }

    case Init: {
      init_fs(fs_file_path);
      break;
    }

    case ReadFs: {
      read_fs(fs_file_path);
      break;
    }

    case Ls: {
      char* path = NULL;
      size_t path_len = 0;
      receive_data(sockd, &path, &path_len);
      ls(fs_file_path, path, sockd);
      free(path);
      break;
    }

    case Mkdir: {
      char* path = NULL;
      size_t path_len = 0;
      receive_data(sockd, &path, &path_len);
      create_dir(fs_file_path, path, sockd);
      break;
    }

    case Touch: {
      char* path = NULL;
      size_t path_len = 0;
      receive_data(sockd, &path, &path_len);
      create_file(fs_file_path, path, sockd);
      break;
    }

    case Open: {
      char* path = NULL;
      size_t path_len = 0;
      receive_data(sockd, &path, &path_len);
      open_file(fs_file_path, path, sockd);
      break;
    }

    case Close: {
      char* fd_to_close_string = NULL;
      size_t fd_to_close_string_len = 0;
      receive_data(sockd, &fd_to_close_string, &fd_to_close_string_len);
      int fd_to_close = strtol(fd_to_close_string, NULL, 10);
      close_file(fs_file_path, fd_to_close, sockd);
      break;
    }

    case Lseek: {
      char* full_string = NULL;
      size_t len = 0;
      receive_data(sockd, &full_string, &len);
      char* next_arg = NULL;
      int fd = strtol(full_string, &next_arg, 10);
      int position = strtol(next_arg, NULL, 10);
      lseek_pos(fs_file_path, fd, position, sockd);
      break;
    }

    case Write: {
      char* full_string = NULL;
      size_t len = 0;
      receive_data(sockd, &full_string, &len);
      char* next_arg = NULL;
      int fd = strtol(full_string, &next_arg, 10);
      char* data = NULL;
      size_t size = strtol(next_arg, &data, 10);
      write_to_file(fs_file_path, fd, data + 1, size, sockd);
      break;
    }

    case Read: {
      char* full_string = NULL;
      size_t len = 0;
      receive_data(sockd, &full_string, &len);
      char* next_arg = NULL;
      int fd = strtol(full_string, &next_arg, 10);
      int size = strtol(next_arg, NULL, 10);
      char data[buffer_length];

      size_t total_read = read_file(fs_file_path, fd, data, size, sockd);
      data[size] = '\0';

      char* buffer = NULL;
      size_t buffer_size = 0;
      char string_buffer[size + 1];
      size_t string_size = sprintf(string_buffer, "\t%zu\t%s", total_read, data);
      buffered_write(&buffer, &buffer_size, string_buffer, strlen(string_buffer));
      send_data(sockd, buffer, buffer_size);
      free(buffer);
      break;
    }

    default: {
      break;
    }

  }

  return true;
}

int server_loop(long port, const char* fs_file_path) {
  int sockd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockd == -1) {
    fprintf(stderr, "Can't create socket. Abort!\n");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(sockd, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
    fprintf(stderr, "Can't create connection. Abort!\n");
    exit(EXIT_FAILURE);
  }

  if (listen(sockd, 1) == -1) {
    fprintf(stderr, "Can't listen. Abort!\n");
    exit(EXIT_FAILURE);
  }

#if __APPLE__
  int kq = kqueue();
  struct kevent change_event[4];
  EV_SET(change_event, sockd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
  if (kevent(kq, change_event, 1, NULL, 0, NULL) == -1) {
    fprintf(stderr, "Can't register kevent. Abort!\n");
    exit(EXIT_FAILURE);
  }
#else
  int epoll_fd = epoll_create1(0);
  struct epoll_event event;
  memset(&event, 0, sizeof(struct epoll_event));
  event.events = EPOLLIN | EPOLLERR | EPOLLHUP;
  event.data.fd = sockd;
  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockd, &event);
#endif

  while (true) {
    int connection_fd = -1;
#if __APPLE__
    struct kevent event[4];
    int new_events = kevent(kq, NULL, 0, event, 1, NULL);
    if (new_events <= 0) {
      continue;
    }

    int event_fd = event[0].ident;

    if (event_fd == sockd) {
      connection_fd = accept(sockd, NULL, NULL);

      if (connection_fd == -1) {
        fprintf(stderr, "Connection error. Abort!\n");
        shutdown(sockd, SHUT_RDWR);
        close(sockd);
        return 1;
      }
    }
#else
    struct epoll_event event;
    memset(&event, 0, sizeof(struct epoll_event));
    int new_events = epoll_wait(epoll_fd, &event, 1, 1000);
    if (new_events <= 0) {
      continue;
    }
    connection_fd = accept(sockd, NULL, NULL);
    if (connection_fd == -1) {
        fprintf(stderr, "Connection error. Abort!\n");
        shutdown(sockd, SHUT_RDWR);
        close(sockd);
        return 1;
    }
#endif
    while (process_command(connection_fd, fs_file_path)) {
    }

    shutdown(connection_fd, SHUT_RDWR);
    close(connection_fd);
  }
  shutdown(sockd, SHUT_RDWR);
  close(sockd);

#if __APPLE__
#else
  close(epoll_fd);
#endif

  return 0;
}