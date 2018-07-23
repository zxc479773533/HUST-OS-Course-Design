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
  syscall(333);
  return 0;
}