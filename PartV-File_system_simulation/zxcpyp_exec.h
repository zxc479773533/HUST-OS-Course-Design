/*
 * zxcpyp exec - Execute function for zxcpyp prompt
 * 
 * Created by zxcpyp at 2018-02-22
 * 
 * Github: zxc479773533
 * 
 * Used for: HUST OS Design - Part V in 2018-08-26
 */

#include "filesystem.h"

/*
 * print_help - Print help messages
 */
void print_help(void) {
  printf("To be add......\n");
}

/*
 * builtin_cmd - Judge buildin command
 */
int developer_cmd(int argc, char **argv) {
  if (!strcmp(argv[0], "reset")) {
    reset_disk();
    return 1;
  }
  if (!strcmp(argv[0], "pino")) {
    print_current_inode_id();
    return 1;
  }
  if (!strcmp(argv[0], "dirnum")) {
    print_current_dir_num();
    return 1;
  }
  if (!strcmp(argv[0], "show")) {
    show_files_info();
    return 1;
  }
  if (argc == 2 && !strcmp(argv[0], "superi")) {
    print_superblk_inode_info(atoi(argv[1]));
    return 1;
  }
  if (argc == 2 && !strcmp(argv[0], "superb")) {
    print_superblk_block_info(atoi(argv[1]));
    return 1;
  }
  /* Not a developer command */
  return 0;
}

/*
 * developer_cmd - Judge developer command
 */
int builtin_cmd(char **argv) {
  if (!strcmp(argv[0], "fmt")) {
    format_disk();
    return 1;
  }
  if (!strcmp(argv[0], "ls")) {
    dir_ls();
    return 1;
  }
  if (!strcmp(argv[0], "exit")) {
    printf("[INFO] User-exit. Terminated!\n");
    close_disk();
    fclose(disk);
    exit(0);
  }
  if (!strcmp(argv[0], "help")) {
    print_help();
    return 1;
  }
  /* Not a built-in command */
  return 0;
}

/*
 * py_execute - The main execute function
 */
int py_execute(char *func , int argc, char **argv) {
  int ret;
  if (argc == 2 && !strcmp(func, "mkdir")) {
    ret = dir_creat(current_inode_id, TYPE_DIR, argv[1]);
    if (ret == FS_FILE_EXIST)
      printf("mkdir: Fail to create directory \"%s\": File already exists\n", argv[1]);
    else if (ret != FS_OK)
      printf("mkdir: Fail to create directory \"%s\": No enough space\n", argv[1]);
    return 1;
  }
  else if (argc == 2 && !strcmp(func, "rmdir")) {
    ret = dir_rm(current_inode_id, TYPE_DIR, argv[1]);
    if (ret == FS_INVALID)
      printf("rmdir: Fail to delete \"%s\": Invalid operation\n", argv[1]);
    else if (ret == FS_NO_EXIST)
      printf("rmdir: Fail to delete \"%s\": File not exists\n", argv[1]);
    else if (ret == FS_NO_PRIVILAGE)
      printf("rmdir: Fail to delete \"%s\": Insufficient privilege\n", argv[1]);
    else if (ret == FS_ISNOT_DIR)
      printf("rmdir: Fail to delete \"%s\": Not a directory\n", argv[1]);
    else if (ret == FS_DIR_NOEMPTY)
      printf("rmdir: Fail to delete \"%s\": Insufficient privilege\n", argv[1]);
    return 1;
  }
  else if (argc == 2 && !strcmp(func, "cd")) {
    int old_inode_id = current_inode_id;
    ret = dir_cd(current_inode_id, argv[1]);
    if (ret == FS_NO_EXIST)
      printf("cd: No such file or directory: \"%s\"\n", argv[1]);
    else if (ret == FS_ISNOT_DIR)
      printf("cd: Not a directory: \"%s\"\n", argv[1]);
    else if (ret == FS_OK)
      path_change(old_inode_id, argv[1]);
    return 1;
  }
  else if (argc == 2 && !strcmp(func, "touch")) {
    ret = dir_creat(current_inode_id, TYPE_FILE, argv[1]);
    if (ret == FS_FILE_EXIST)
      mtime_change(current_inode_id, argv[1]);
    else if (ret != FS_OK)
      printf("touch: Fail to create file \"%s\": No enough space\n", argv[1]);
    return 1;
  }
  else if (argc == 2 && !strcmp(func, "rm")) {
    ret = dir_rm(current_inode_id, TYPE_FILE, argv[1]);
    if (ret == FS_INVALID)
      printf("rmdir: Fail to delete \"%s\": Invalid operation\n", argv[1]);
    else if (ret == FS_NO_EXIST)
      printf("rmdir: Fail to delete \"%s\": File not exists\n", argv[1]);
    else if (ret == FS_NO_PRIVILAGE)
      printf("rmdir: Fail to delete \"%s\": Insufficient privilege\n", argv[1]);
    else if (ret == FS_ISNOT_FILE)
      printf("rmdir: Fail to delete \"%s\": Not a file\n", argv[1]);
    return 1;
  }
  return 0;
}