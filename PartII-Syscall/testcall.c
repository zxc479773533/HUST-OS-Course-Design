/*
 * HUST OS Design - Part II
 * 
 * Syscall - test syscall testcall
 * 
 * Created by zxcpyp at 2018-07-22
 * 
 * Github: zxc479773533
 */

#include <unistd.h>
#include <stdio.h>
#include <errno.h>

int main(void) {
  printf("/*\n");
  printf(" * HUST OS Design - Part II\n");
  printf(" *\n");
  printf(" * Test01: Test syscall\n");
  printf(" * issue: dmesg to see \"Hello world!\"\n");
  printf(" */\n");
  syscall(333);
  return 0;
}