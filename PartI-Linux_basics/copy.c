/*
 * HUST OS Design - Part I
 * 
 * Linux Basics - File copy
 * 
 * Created by zxcpyp at 2018-07-17
 * 
 * Github: zxc479773533
 */

#include "../lib/zxcpyp_sys.h"

#define BUF_LEN 1024

mode_t get_file_mode(char *file) {
  struct stat s_buf;
  mode_t file_mode = 0x0;
  if (stat(file, &s_buf) != 0)
    err_exit("Stat");
  return s_buf.st_mode;
}

void do_copy(int read_fd, int write_fd) {
  int read_num;
  char buf[BUF_LEN];
  for (;;) {
    read_num = read(read_fd, buf, BUF_LEN);
    if (read_num == -1)
      err_exit("Copy");
    else if (read_num == 0)
      break;
    write(write_fd, buf, read_num);
  }
}

int main(int argc, char **argv) {
  int read_fd, write_fd;
  /* Check args */
  if (argc != 3)
    usage_err("./copy <src> <dst>");
  /* Open files */
  if ((read_fd = open(argv[1], O_RDONLY)) == -1)
    err_exit("Open source file");
  if ((write_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, get_file_mode(argv[1]))) == -1)
    err_exit("Open destnation file");
  do_copy(read_fd, write_fd);
  close(read_fd);
  close(write_fd);
  return 0;
}