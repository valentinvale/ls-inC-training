# LSC

LSC is a simplified implementation of the Unix `ls` command written in C.

The program can list the contents of a directory, display hidden files and show detailed information about each file.

Two source files are provided:

- `lsc.c` — Linux version
- `lscwin.c` — Windows version

## Features

The program supports:

- listing files from the current directory;
- listing files from a specified directory;
- alphabetical sorting;
- displaying hidden files using `-a`;
- long format output using `-l`;
- combining options, such as `-la` or `-al`;
- aligned columns in long format;
- displaying file permissions;
- displaying file size;
- displaying modification time;
- displaying hard link count;
- displaying owner and group on Linux.