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
  if (!strcmp(argv[0], "puid")) {
    print_current_user_id();
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
  if (!strcmp(argv[0], "users")) {
    show_users_info();
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
    if (current_user_id == 0)
      format_disk();
    else
      printf("fmt: You need root privilege!\n");
    return 1;
  }
  if (!strcmp(argv[0], "passwd")) {
    user_pwd();
    return 1;
  }
  if (!strcmp(argv[0], "exit")) {
    printf("[EXIT] User-exit. Terminated!\n");
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
  if (!strcmp(func, "mkdir")) {
    if (argc != 2) {
      printf("Usage: mkdir [dirname]\n");
      return 1;
    }
    ret = dir_creat(current_inode_id, TYPE_DIR, argv[1]);
    if (ret == FS_FILE_EXIST)
      printf("mkdir: Fail to create directory \"%s\": File already exists\n", argv[1]);
    else if (ret == FS_NO_PRIVILAGE)
      printf("mkdir: Fail to create directory \"%s\": Insufficient privilege\n", argv[1]);
    else if (ret != FS_OK)
      printf("mkdir: Fail to create directory \"%s\": No enough space\n", argv[1]);
    return 1;
  }
  if (!strcmp(func, "rmdir")) {
    if (argc != 2) {
      printf("Usage: rmdir [dirname]\n");
      return 1;
    }
    ret = dir_rm(current_inode_id, TYPE_DIR, argv[1]);
    if (ret == FS_INVALID)
      printf("rmdir: Fail to delete \"%s\": Invalid operation\n", argv[1]);
    else if (ret == FS_NO_PRIVILAGE)
      printf("rmdir: Fail to delete \"%s\": Insufficient privilege\n", argv[1]);
    else if (ret == FS_NO_EXIST)
      printf("rmdir: Fail to delete \"%s\": File not exists\n", argv[1]);
    else if (ret == FS_NO_PRIVILAGE)
      printf("rmdir: Fail to delete \"%s\": Insufficient privilege\n", argv[1]);
    else if (ret == FS_ISNOT_DIR)
      printf("rmdir: Fail to delete \"%s\": Not a directory\n", argv[1]);
    else if (ret == FS_DIR_NOEMPTY)
      printf("rmdir: Fail to delete \"%s\": Directory not empty\n", argv[1]);
    return 1;
  }
  if (!strcmp(func, "cd")) {
    if (argc != 2) {
      printf("Usage: cd [dirname]\n");
      return 1;
    }
    int old_inode_id = current_inode_id;
    ret = dir_cd(current_inode_id, argv[1]);
    if (ret == FS_NO_EXIST)
      printf("cd: No such file or directory: \"%s\"\n", argv[1]);
    else if (ret == FS_NO_PRIVILAGE)
      printf("cd: Insufficient privilege: \"%s\"\n", argv[1]);
    else if (ret == FS_ISNOT_DIR)
      printf("cd: Not a directory: \"%s\"\n", argv[1]);
    else if (ret == FS_OK)
      path_change(old_inode_id, argv[1]);
    return 1;
  }
  if (!strcmp(func, "ls")) {
    if (argc == 2 && !strcmp(argv[1], "-l"))
      dir_ls_l();
    else
      dir_ls();
    return 1;
  }
  if (!strcmp(func, "touch")) {
    if (argc != 2) {
      printf("Usage: touch [filename]\n");
      return 1;
    }
    ret = dir_creat(current_inode_id, TYPE_FILE, argv[1]);
    if (ret == FS_FILE_EXIST)
      mtime_change(current_inode_id, argv[1]);
    else if (ret == FS_NO_PRIVILAGE)
      printf("touch: Fail to create file \"%s\": Insufficient privilege\n", argv[1]);
    else if (ret != FS_OK)
      printf("touch: Fail to create file \"%s\": No enough space\n", argv[1]);
    return 1;
  }
  if (!strcmp(func, "rm")) {
    if (argc != 2) {
      printf("Usage: rm [filename]\n");
      return 1;
    }
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
  if (!strcmp(func, "vim")) {
    if (argc != 2) {
      printf("Usage: vim [filename]\n");
      return 1;
    }
    int pid, status;
	  char *vim_arg[] = {"vim", BUFFERFILE, NULL};
    if (check_if_readonly(current_inode_id, argv[1]) == TRUE) {
      printf("vim: Fail to save \"%s\": Insufficient privilege\n", argv[1]);
      return 1;
    }
    ret = file_open(current_inode_id, argv[1]);
    if (ret == FS_IS_DIR) {
      printf("vim: Fail to open \"%s\": Is a directory\n", argv[1]);
      return 1;
    }
    else if (ret == FS_NO_PRIVILAGE) {
      printf("vim: Fail to open \"%s\": Insufficient privilege\n", argv[1]);
      return 1;
    }
    else if (ret == FS_NO_EXIST) {
      ret = dir_creat(current_inode_id, TYPE_FILE, argv[1]);
      if (ret == FS_NO_PRIVILAGE) {
        printf("vim: Fail to creat \"%s\": Insufficient privilege\n", argv[1]);
        return 1;
      }
      file_open(current_inode_id, argv[1]);
    }
    if((pid = fork()) == 0) {
			execvp("vim", vim_arg);
		}
    wait(&status);
    file_close(current_inode_id, argv[1]);
    return 1;
  }
  if (!strcmp(func, "cat")) {
    if (argc != 2) {
      printf("Usage: cat [filename]\n");
      return 1;
    }
    ret = file_open(current_inode_id, argv[1]);
    if (ret == FS_IS_DIR)
      printf("cat: \"%s\": Is a directory\n", argv[1]);
    else if (ret == FS_NO_EXIST)
      printf("cat: \"%s\": No such file or directory\n", argv[1]);
    else if (ret == FS_NO_PRIVILAGE)
      printf("cat: \"%s\": Insufficient privilege\n", argv[1]);
    else {
      file_cat();
      file_close(current_inode_id, argv[1]);
    }
    return 1;
  }
  if (!strcmp(func, "useradd")) {
    if (argc != 3) {
      printf("Usage: useradd [username] [userpwd]\n");
      return 1;
    }
    if (current_user_id != 0)
      printf("useradd: You need root privilege!\n");
    else {
      ret = user_add(argv[1], argv[2]);
      if (ret == FS_USER_EXIST)
        printf("useradd: Failed to add user \"%s\": User already exists\n", argv[1]);
    }
    return 1;
  }
  if (!strcmp(func, "userdel")) {
    if (argc != 2) {
      printf("Usage: userdel [username]\n");
      return 1;
    }
    if (current_user_id != 0)
      printf("userdel: You need root privilege!\n");
    else {
      ret = user_del(argv[1]);
      if (ret == FS_USER_NOT_EXIST)
        printf("userdel: Failed to delete user \"%s\": User not exists\n", argv[1]);
    }
    return 1;
  }
  if (!strcmp(func, "chmod")) {
    if (argc != 3) {
      printf("Usage: userdel [mod] [filename]\n");
      return 1;
    }
    ret = mode_change(atoi(argv[1]), argv[2]);
    if (ret == FS_NO_PRIVILAGE)
      printf("chmod: Change the permissions of \"%s\": Invalid operation\n", argv[2]);
    else if (ret == FS_NO_EXIST)
      printf("chmod: Unable to access \"%s\": No such file or directory\n", argv[2]);
    else if (ret == FS_INVALID_MODE)
      printf("chmod: Invalid operation: \"%s\"\n", argv[1]);
    return 1;
  }
  if (!strcmp(func, "mv")) {
    if (argc != 3) {
      printf("Usage: mv [srcfile] [dstfile | dstdir/]\n");
      return 1;
    }
    ret = file_mv(current_inode_id, argv[1], argv[2]);
    if (ret == FS_NO_EXIST)
      printf("mv: Unable to get file status for \"%s\" or \"%s\" (stat) : No such file or directory\n", argv[1], argv[2]);
    else if (ret == FS_NO_PRIVILAGE)
      printf("mv: Unable to move \"%s\": Insufficient privilege\n", argv[1]);
    else if (ret == FS_IS_DIR)
      printf("mv: Unable to move \"%s\": Not a file\n", argv[1]);
    else if (ret == FS_IS_FILE)
      printf("mv: Unable to move \"%s\" into \"%s\": Not a directory\n", argv[1], argv[2]);
    else if (ret == FS_FILE_EXIST)
      printf("mv: Unable to move \"%s\": Target file exists\n", argv[1]);
    return 1;
  }
  if (!strcmp(func, "cp")) {
    if (argc != 3) {
      printf("Usage: cp [srcfile] [dstfile | dstdir/]\n");
      return 1;
    }
    ret = file_cp(current_inode_id, argv[1], argv[2]);
    if (ret == FS_NO_EXIST)
      printf("cp: Unable to get file status for \"%s\" or \"%s\" (stat) : No such file or directory\n", argv[1], argv[2]);
    else if (ret == FS_NO_PRIVILAGE)
      printf("cp: Unable to copy \"%s\": Insufficient privilege\n", argv[1]);
    else if (ret == FS_IS_DIR)
      printf("cp: Unable to copy \"%s\": Not a file\n", argv[1]);
    else if (ret == FS_IS_FILE)
      printf("cp: Unable to copy \"%s\" into \"%s\": Not a directory\n", argv[1], argv[2]);
    else if (ret == FS_FILE_EXIST)
      printf("cp: Unable to move \"%s\": Target file exists\n", argv[1]);
    return 1;
  }
  return 0;
}
