/*
 * HUST OS Design - Part II
 * 
 * Syscall - test syscall mycopy
 * 
 * Created by zxcpyp at 2018-07-22
 * 
 * Github: zxc479773533
 */

#include <unistd.h>
#include <stdio.h>
#include <errno.h>

int main(int argc, char **argv) {
  printf("Copy: %s -> %s\n", argv[1], argv[2]);
  long ret = syscall(335, argv[1], argv[2]);
  printf("ret: %ld\n", ret);
  printf("errno: %d\n", errno);
  return 0;
}