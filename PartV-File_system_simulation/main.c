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

int main(int argc, char **argv) {
  disk = fopen(FS_PATH, "r+");
  if (disk == NULL)
    return FS_NO_EXIST;
  Start_Shell(argc, argv);
  return 0;
}