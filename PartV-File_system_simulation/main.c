/*
 * HUST OS Design - Part V
 * 
 * File system simulation - Main
 * 
 * Created by zxcpyp at 2018-08-26
 * 
 * Github: zxc479773533
 */

#include "zxcpyp_prompt.h"

#define FS_PATH "../PartV-File_system_simulation/zxcpyp_disk"

void print_version() {
  char load[] = "....................";
  char ch[20];
  printf("ZXCPYP File System: version v1.2\n\n");
  printf("Copyright (C) 2018 zxcpyp\n\n");
  for (int i = 0; i < 19; i++) {
    memset(ch, 0, sizeof(ch));
    strncpy(ch, load, i + 1);
    printf("loading %s\n", ch);
    fflush(stdout);
    usleep(100000);
    printf("\033[1A\033[K");
  }
  printf("loading ....................\n\n");
}

int main(int argc, char **argv) {
  int ret;
  print_version();
  disk = fopen(FS_PATH, "r+");
  if (disk == NULL)
    return FS_NO_EXIST;
  while (login() != FS_LOGIN) {}
  ret = load_super_block();
  if (ret == FS_RD_ERROR)
    return FS_RD_ERROR;
  Start_Shell(argc, argv);
  return 0;
}