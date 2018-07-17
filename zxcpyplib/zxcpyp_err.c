/*
 * zxcpyp's lib
 *
 * zxcpyp error message lib
 */

#include "zxcpyp_err.h"

void err_msg(const char *err) {
  printf("Error: %s\n", err);
}

void err_exit(const char *err) {
  printf("%s error!\n", err);
  printf("Exit with 1.\n");
  exit(1);
}

void usage_err(const char *err) {
  printf("Usage: %s\n", err);
  exit(1);
}

void fatal_err(const char *err) {
  printf("Fatal: %s\n", err);
  exit(1);
}
