## Mini version of EXT
[![Build Status](https://travis-ci.com/yaishenka/ext.svg?branch=minifs-net)](https://travis-ci.com/yaishenka/ext)


[Documentation](https://yaishenka.github.io/ext/)

# Commands

`help` - print help command

`quit` - command to quit FS

`ls [path]` - list directory contents

`init` - init file system

`read_fs` - read fs_file and checks it

`mkdir [path]` - make directories

`touch [path]` - create files

`open [path]` - open file and return FD

`close [fd]` - close FD

`write [fd] [data]` - write data to FD

`write_from [fd] [path]` - read data from path and write to FD

`read [fd] [size]` - read size bytes from FD

`read_to [fd] [path] [size]` - read file from fd.pos and write data to path. If size not specified file will be readed till end

`lseek [fd] [pos]` - set fd.pos = pos

# Usage

    mkdir build
    cd build
    cmake ..
    make

In first terminal:

    cd FileSystem
    ./server

In second:

    cd client
    ./client
