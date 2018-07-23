/*
 * HUST OS Design - Part II
 * 
 * Syscall - test syscall teststr
 * 
 * Created by zxcpyp at 2018-07-22
 * 
 * Github: zxc479773533
 */

#include <unistd.h>
#include <stdio.h>
#include <errno.h>

int main(int argc, char **argv) {
  printf("Arg: %s\n", argv[1]);
  long ret = syscall(334, argv[1]);
  printf("ret: %ld\n", ret);
  printf("errno: %d\n", errno);
  return 0;
}