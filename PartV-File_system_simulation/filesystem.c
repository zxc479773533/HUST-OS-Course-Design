/*
 * HUST OS Design - Part V
 * 
 * File system simulation - Implementation
 * 
 * Created by zxcpyp at 2018-08-26
 * 
 * Github: zxc479773533
 */

#include "filesystem.h"

FILE *disk;
super_block super;
int current_inode_id;
inode current_inode;
directory current_dir_content[DIRMAXNUM];
int current_dir_num;
int current_user_id;
char path[128];


/********************DEVELOPER FUNCTIONS********************/
/*
 * Warning: These functions is only for developer, they will not
 *          be called in the formal edition.
 */


/*
 * reset_disk - Totally reset the disk, you can call it to reset
 *              disk or make a new file become a disk file for this
 *              program.
 * 
 * root initial password: 123456
 */
void reset_disk(void) {
  sys_users all_users;
  /* Reset user */
  fseek(disk, 0, SEEK_SET);
  fread(&all_users, sizeof(sys_users), 1, disk);
  memset(&all_users, 0, sizeof(sys_users));

  /* Set root initial password */
  all_users.user_map[0] = 1;
  all_users.user_num = 1;
  strcpy(all_users.users[0].user_name, "root");
  strcpy(all_users.users[0].user_pwd, "123456");

  /* Save data */
  fseek(disk, 0, SEEK_SET);
  fwrite(&all_users, sizeof(sys_users), 1, disk);

  /* Format the disk */
  format_disk();
}

/*
 * print_current_user_id - See the current user id
 */
void print_current_user_id(void) {
  printf("%d\n", current_user_id);
}


/*
 * print_current_inode_id - See the current inode id
 */
void print_current_inode_id(void) {
  printf("%d\n", current_inode_id);
}

/*
 * print_current_inode_id - See the current directory num
 */
void print_current_dir_num(void) {
  printf("%d\n", current_dir_num);
}

/*
 * show_files_info - See the files inode information in directory
 */
void show_files_info(void) {
  int pos;
  inode node;
  for (pos = 0; pos < current_dir_num; pos++) {
    fseek(disk, INODEPOS + current_dir_content[pos].inode_id * INODESIZE, SEEK_SET);
    fread(&node, sizeof(node), 1, disk);
    printf("pos: %d  ", pos);
    printf("name: %-10s  ", current_dir_content[pos].name);
    printf("inode id: %d  ", current_dir_content[pos].inode_id);
    printf("user id: %d\n\n", node.user_id);
  }
}

/*
 * print_superblk_inode_info - See inode information in super block
 */
void print_superblk_inode_info(int pos) {
  printf("Super block inode:\n");
  printf("Pos %d: %d\n", pos, super.inode_map[pos]);
  printf("Free num: %d\n", super.inode_free_num);
}

/*
 * print_superblk_block_info - See block information in super block
 */
void print_superblk_block_info(int pos) {
  printf("Super block block:\n");
  printf("Pos %d: %d\n", pos, super.block_map[pos]);
  printf("Free num: %d\n", super.block_free_num);
}

void show_users_info(void) {
  int pos;
  sys_users all_users;

  /* Read users inforation */
  fseek(disk, 0, SEEK_SET);
  fread(&all_users, sizeof(sys_users), 1, disk);

  for (pos = 0; pos < USERNUMMAX; pos++) {
    if (all_users.user_map[pos] == 1) {
      printf("User id: %d\n", pos);
      printf("User id: %s\n\n", all_users.users[pos].user_name);
    }
  }
}

/********************CORE FUNCTIONS********************/

/*
 * inode_alloc - Alloc a new free inode from inode map
 */
int inode_alloc(void) {
  int ino;
  if (super.inode_free_num <= 0)
    return FS_NO_INODE;
  super.inode_free_num--;
  for (ino = 0; ino < INODENUM; ino++) {
    if (super.inode_map[ino] == 0) {
      super.inode_map[ino] = 1;
      break;
    }
  }
  return ino;
}

/*
 * inode_free - Free a inode and the file's all block
 */
int inode_free(int ino) {
  inode node;
  fseek(disk, INODEPOS + ino * INODESIZE, SEEK_SET);
  fread(&node, sizeof(inode), 1, disk);
  for (int i = 0; i < node.block_used_num; i++)
    block_free(node.block_used[i]);
  super.inode_map[ino] = 0;
  super.inode_free_num++;
  return FS_OK;
}

/*
 * init_dir_inode - Initial directory inode
 */
int init_dir_inode(int new_ino, int ino) {
  int bno;
  inode node;
  directory basic_link[2];

  fseek(disk, INODEPOS + new_ino * INODESIZE, SEEK_SET);
  fread(&node, sizeof(inode), 1, disk);
  bno = block_alloc();

  /* Set new inode information */
  node.block_used[0] = bno;
  node.block_used_num = 1;
  node.size = 2 * sizeof(directory);
  node.mode = oct2dec(1755);
  time_t timer;
	time(&timer);
  node.creat_time = timer;
  node.modify_time = timer;
  node.user_id = current_user_id;

  /* Save inode information */
  fseek(disk, INODEPOS + new_ino * INODESIZE, SEEK_SET);
  fwrite(&node, sizeof(inode), 1, disk);

  /* Set basic links */
  strcpy(basic_link[0].name, ".");
  basic_link[0].inode_id = new_ino;
  strcpy(basic_link[1].name, "..");
  basic_link[1].inode_id = ino;

  /* Save basic links */
  fseek(disk, BLOCKPOS + bno * BLOCKSIZE, SEEK_SET);
  fwrite(basic_link, sizeof(directory), 2, disk);

  return FS_OK;
}

/*
 * init_file_inode - Initial file inode
 */
int init_file_inode(int new_ino) {
  inode node;
  fseek(disk, INODEPOS + new_ino * INODESIZE, SEEK_SET);
  fread(&node, sizeof(inode), 1, disk);

  /* Set new inode information */
  node.block_used_num = 0;
  node.size = 0;
  node.mode = oct2dec(644);
  time_t timer;
	time(&timer);
  node.creat_time = timer;
  node.modify_time = timer;
  node.user_id = current_user_id;

  /* Save inode information */
  fseek(disk, INODEPOS + new_ino * INODESIZE, SEEK_SET);
  fwrite(&node, sizeof(inode), 1, disk);

  return FS_OK;
}

/*
 * block_alloc - Alloc a new free block from block map
 */
int block_alloc(void) {
  int bno;
  if (super.block_free_num <= 0)
    return FS_NO_BLOCK;
  super.block_free_num--;
  for (bno = 0; bno < BLOCKNUM; bno++) {
    if (super.block_map[bno] == 0) {
      super.block_map[bno] = 1;
      break;
    }
  }
  return bno;
}

/*
 * block_free - Free a block
 */
int block_free(int bno) {
  super.block_free_num++;
  super.block_map[bno] = 0;
  return FS_OK;
}


/********************USER FUNCTIONS********************/


/*
 * login - Login for the disk
 */
int login(void) {
  sys_users all_users;
  char uname[USERNAMEMAX];
  char upwd[USERPWDMAX];
  printf("localhost login: ");
  scanf("%s", uname);
  printf("\nPassword: ");
  scanf("%s", upwd);
  getchar();

  /* Read users inforation */
  fseek(disk, 0, SEEK_SET);
  fread(&all_users, sizeof(sys_users), 1, disk);

  /* Check user */
  for (int i = 0; i < USERNUMMAX; i++) {
    if (strcmp(all_users.users[i].user_name, uname) == 0) {
      if (strcmp(all_users.users[i].user_pwd, upwd) == 0) {
        current_user_id = i;
        sprintf(path, "%s@localhost: / >", uname);
        printf("\033[2J");
        return FS_LOGIN;
      }
    }
  }

  usleep(500000);
  printf("Login incorrect\n\n");
  return FS_LOGIN_ERROR;
}

/*
 * user_pwd - Change a user's password
 */
int user_pwd(void) {
  int i;
  sys_users all_users;
  char current_pwd[USERPWDMAX], new_pwd[USERPWDMAX], new_pwd_2[USERPWDMAX];
  
  /* Read users inforation */
  fseek(disk, 0, SEEK_SET);
  fread(&all_users, sizeof(sys_users), 1, disk);

  printf("Change password for %s\n", all_users.users[current_user_id].user_name);
  printf("Current password: ");
  scanf("%s", current_pwd);
  getchar();

  if (strcmp(all_users.users[current_user_id].user_pwd, current_pwd) != 0) {
    printf("passwd: Identification failure\n");
    printf("passwd: Password not changed\n");
    return FS_INVALID;
  }

  printf("New password: ");
  scanf("%s", new_pwd);
  printf("New password again: ");
  scanf("%s", new_pwd_2);
  getchar();

  if (strcmp(new_pwd, new_pwd_2) != 0) {
    printf("Sorry, the password does not match\n");
    printf("passwd: Password service preliminary check failed\n");
    printf("passwd: Password not changed\n");
    return FS_INVALID;
  }

  strcpy(all_users.users[current_user_id].user_pwd, new_pwd);
  printf("password: Password successfully updated\n");

  /* Save users inforation */
  fseek(disk, 0, SEEK_SET);
  fwrite(&all_users, sizeof(sys_users), 1, disk);

  return FS_OK;
}

/*
 * user_add - Add a user
 */
int user_add(char *name, char *pwd) {
  sys_users all_users;

  /* Read users inforation */
  fseek(disk, 0, SEEK_SET);
  fread(&all_users, sizeof(sys_users), 1, disk);

  /* Check user */
  for (int i = 0; i < USERNUMMAX; i++) {
    if (strcmp(all_users.users[i].user_name, name) == 0) {
      return FS_USER_EXIST;
    }
  }

  for (int i = 0; i < USERNUMMAX; i++) {
    if (all_users.user_map[i] == 0) {
      all_users.user_map[i] = 1;
      strcpy(all_users.users[i].user_name, name);
      strcpy(all_users.users[i].user_pwd, pwd);
      break;
    }
  }

  /* Save users inforation */
  fseek(disk, 0, SEEK_SET);
  fwrite(&all_users, sizeof(sys_users), 1, disk);

  return FS_OK;
}


/*
 * user_del - Delete a user
 */
int user_del(char *name) {
  sys_users all_users;
  int i;

  /* Read users inforation */
  fseek(disk, 0, SEEK_SET);
  fread(&all_users, sizeof(sys_users), 1, disk);

  /* Check user */
  for (i = 0; i < USERNUMMAX; i++) {
    if (strcmp(all_users.users[i].user_name, name) == 0) {
      memset(all_users.users[i].user_name, 0, USERNAMEMAX);
      memset(all_users.users[i].user_pwd, 0, USERPWDMAX);
      all_users.user_map[i] = 0;
      break;
    }
  }
  if (i == USERNUMMAX)
    return FS_USER_NOT_EXIST;

  /* Save users inforation */
  fseek(disk, 0, SEEK_SET);
  fwrite(&all_users, sizeof(sys_users), 1, disk);

  return FS_OK;
}


/********************DISK FUNCTIONS********************/


/*
 * load_super_block - Load super block information from disk
 */
int load_super_block(void) {
  int ret;

  /* Load super block */
  fseek(disk, SUPERPOS, SEEK_SET);
  ret = fread(&super, sizeof(super_block), 1, disk);
  if (ret != 1)
    return FS_RD_ERROR;

  /* Open root dir directory */
  current_inode_id = 0;
  ret = dir_open(current_inode_id);
  if (ret != FS_OK)
    return ret;

  return FS_OK;
}

/*
 * format_disk - Format and initialize the disk
 */
int format_disk(void) {
  int ret;

  /* Set inode map */
  memset(super.inode_map, 0, sizeof(super.inode_map));
  super.inode_map[0] = 1;
  super.inode_free_num = INODENUM - 1;

  /* Set block map */
  memset(super.block_map, 0, sizeof(super.block_map));
  super.block_map[0] = 1;
  super.block_free_num = BLOCKNUM - 1;

  /* Set root inode */
  current_inode_id = 0;
  fseek(disk, INODEPOS, SEEK_SET);
  ret = fread(&current_inode, sizeof(inode), 1, disk);
  if (ret != 1)
    return FS_RD_ERROR;
  current_inode.block_used[0] = 0;
  current_inode.block_used_num = 1;
  current_inode.size = 2 * sizeof(directory);
  current_inode.mode = oct2dec(1755);
  time_t timer;
	time(&timer);
  current_inode.creat_time = timer;
  current_inode.modify_time = timer;
  current_inode.user_id = 0;

  /* Set basic link of root file */
  current_dir_num = 2;
  strcpy(current_dir_content[0].name, ".");
  current_dir_content[0].inode_id = 0;
  strcpy(current_dir_content[1].name, "..");
  current_dir_content[1].inode_id = 0;

  strcpy(path, "root@localhost: / >");

  return FS_OK;
}


/*
 * close_disk - Load super block information from disk
 */
int close_disk(void) {
  int ret;

  /* Save super block */
  fseek(disk, SUPERPOS, SEEK_SET);
  ret = fwrite(&super, sizeof(super_block), 1, disk);
  if (ret != 1)
    return FS_WR_ERROR;

  /* Close current directory */
  ret = dir_close(current_inode_id);
  if (ret != 1)
    return ret;

  return FS_OK;
}


/********************DIR FUNCTIONS********************/


/*
 * dir_open - Open a directory, and read its data in memory
 */
int dir_open(int ino) {
  int i, ret;
  int end_block_dirnum;
  fseek(disk, INODEPOS + ino * INODESIZE, SEEK_SET);
  ret = fread(&current_inode, sizeof(inode), 1, disk);
  if (ret != 1)
    return FS_RD_ERROR;

  /* Read all directory entry */
  for (i = 0; i < current_inode.block_used_num - 1; i++) {
    fseek(disk, BLOCKPOS + current_inode.block_used[i] * BLOCKSIZE, SEEK_SET);
    fread(current_dir_content + i * DIRMAXINBLK, sizeof(directory), DIRMAXINBLK, disk);
  }
  end_block_dirnum = current_inode.size / sizeof(directory) - DIRMAXINBLK * (current_inode.block_used_num - 1);
  fseek(disk, BLOCKPOS + current_inode.block_used[i] * BLOCKSIZE, SEEK_SET);
  fread(current_dir_content + i * DIRMAXINBLK, sizeof(directory), end_block_dirnum, disk);

  time_t timer;
  time(&timer);
  current_inode.modify_time = timer;
  current_dir_num = i * DIRMAXINBLK + end_block_dirnum;
  return FS_OK;
}

/*
 * dir_close - Close a directory, and write its data back to disk
 */
int dir_close(int ino) {
  int i, ret;
  int end_block_dirnum;

  /* Write all directory entry back to disk */
  for (i = 0; i < current_inode.block_used_num - 1; i++) {
    fseek(disk, BLOCKPOS + current_inode.block_used[i] * BLOCKSIZE, SEEK_SET);
    fwrite(current_dir_content + i * DIRMAXINBLK, sizeof(directory), DIRMAXINBLK, disk);
  }
  end_block_dirnum = current_dir_num - i * DIRMAXINBLK;
  fseek(disk, BLOCKPOS + current_inode.block_used[i] * BLOCKSIZE, SEEK_SET);
  fwrite(current_dir_content + i * DIRMAXINBLK, sizeof(directory), end_block_dirnum, disk);

  current_inode.size = current_dir_num * sizeof(directory);
  fseek(disk, INODEPOS + ino * INODESIZE, SEEK_SET);
  ret = fwrite(&current_inode, sizeof(inode), 1, disk);
  if (ret != 1)
    return FS_WR_ERROR;
  return FS_OK;
}

/*
 * dir_creat - Creat a directory or a file and initial the inode
 */
int dir_creat(int ino, int type, char *name) {
  int new_ino;
  int block_need = 1;
  if (current_dir_num >= DIRMAXNUM)
    return FS_DIR_FULL;
  if (check_name(name) != FS_OK)
    return FS_FILE_EXIST;
  
  /* Check mode */
  if (check_mode(ino, CAN_WRITE) == 0)
    return FS_NO_PRIVILAGE;

  /* If need more blocks */
  if (current_dir_num / DIRMAXINBLK != (current_dir_num + 1) / DIRMAXINBLK)
    block_need++;
  if (block_need > super.block_free_num)
    return FS_NO_BLOCK;
  if (block_need == 2)
    current_inode.block_used[++current_inode.block_used_num] = block_alloc();
  new_ino = inode_alloc();
  if (new_ino == FS_NO_INODE)
    return FS_NO_INODE;

  /* Initial new inode */
  if (type == TYPE_DIR)
    init_dir_inode(new_ino, ino);
  else
    init_file_inode(new_ino);

  /* Register new inode */
  strcpy(current_dir_content[current_dir_num].name, name);
  current_dir_content[current_dir_num].inode_id = new_ino;

  /* Update modify time */
  time_t timer;
	time(&timer);
  current_inode.modify_time = timer;

  current_dir_num++;
  return FS_OK;
}

/*
 * dir_rm - Delete a empty directory or a file
 */
int dir_rm(int ino, int type, char *name) {
  int rm_inode;
  int ret;
  inode node;

  /* Check mode */
  if (check_mode(ino, CAN_WRITE) == 0)
    return FS_NO_PRIVILAGE;

  /* Can't delete . and .. */
  if (!strcmp(name, ".") || !strcmp(name, ".."))
    return FS_INVALID;
  
  /* Check if the directory or file exists */
  for (rm_inode = 0; rm_inode < current_dir_num; rm_inode++) {
    if (strcmp(name, current_dir_content[rm_inode].name) == 0)
      break;
  }
  if (rm_inode == current_dir_num)
    return FS_NO_EXIST;

  rm_inode = current_dir_content[rm_inode].inode_id;
  /* Read inode information */
  fseek(disk, INODEPOS + rm_inode * INODESIZE, SEEK_SET);
  fread(&node, sizeof(node), 1, disk);

  /* Check user */
  if (node.user_id != current_user_id && current_user_id != 0)
    return FS_NO_PRIVILAGE;

  /* Check type */
  ret = check_type(node.mode, type);
  if (ret == FS_ISNOT_DIR || ret == FS_ISNOT_FILE)
    return ret;

  if (ret == FS_IS_DIR) {
    dir_cd(ino, name);
    if (current_dir_num != 2) {
      dir_cd(rm_inode, "..");
      return FS_DIR_NOEMPTY;
    }
    dir_cd(rm_inode, "..");
  }

  int pos;
  inode_free(rm_inode);
  for (pos = 0; pos < current_dir_num; pos++) {
    if (strcmp(current_dir_content[pos].name, name) == 0)
      break;
  }
  for (; pos < current_dir_num - 1; pos++) {
    current_dir_content[pos] = current_dir_content[pos + 1];
  }
  current_dir_num--;

  /* Free last block if need */
  if (current_dir_num / DIRMAXINBLK != (current_dir_num - 1) / DIRMAXINBLK) {
    current_inode.block_used_num--;
    block_free(current_inode.block_used[current_inode.block_used_num]);
  }

  /* Update modify time */
  time_t timer;
	time(&timer);
  current_inode.modify_time = timer;

  return FS_OK;
}

/*
 * dir_cd - Enter into a directory
 */
int dir_cd(int ino, char *name) {
  int i;
  int cd_inode;
  inode node;

  /* Check if the directory or file exists */
  for (i = 0; i < current_dir_num; i++) {
    if (strcmp(current_dir_content[i].name, name) == 0)
      break;
  }
  if (i == current_dir_num)
    return FS_NO_EXIST;
  cd_inode = current_dir_content[i].inode_id;

  /* Check if this is a directory */
  fseek(disk, INODEPOS + cd_inode * INODESIZE, SEEK_SET);
  fread(&node, sizeof(node), 1, disk);
  if (check_type(node.mode, TYPE_FILE) == FS_IS_FILE)
    return FS_ISNOT_DIR;
  
  /* Check mode */
  if (check_mode(cd_inode, CAN_READ) == 0)
    return FS_NO_PRIVILAGE;

  dir_close(ino);
  current_inode_id = cd_inode;
  dir_open(cd_inode);

  return FS_OK;
}

/*
 * dir_ls - List all files in directory
 */
int dir_ls(void) {
  /* Save current status */
  dir_close(current_inode_id);
  dir_open(current_inode_id);

  int pos;
  inode node;
  for (pos = 0; pos < current_dir_num; pos++) {
    fseek(disk, INODEPOS + current_dir_content[pos].inode_id * INODESIZE, SEEK_SET);
    fread(&node, sizeof(node), 1, disk);
    if (check_type(node.mode, TYPE_DIR) == FS_IS_DIR)
      printf("\e[1;34m%s\t\e[0m", current_dir_content[pos].name);
    else
      printf("%s\t", current_dir_content[pos].name);
  }
  printf("\n");

  return FS_OK;
}

/*
 * dir_ls_l - List all files and its information in a directory
 */
int dir_ls_l(void) {
  /* Save current status */
  dir_close(current_inode_id);
  dir_open(current_inode_id);

  int pos;
  sys_users all_users;
  inode node;
  char modstr[11];
  char *time;

  /* Read users inforation */
  fseek(disk, 0, SEEK_SET);
  fread(&all_users, sizeof(sys_users), 1, disk);

  for (pos = 0; pos < current_dir_num; pos++) {
    fseek(disk, INODEPOS + current_dir_content[pos].inode_id * INODESIZE, SEEK_SET);
    fread(&node, sizeof(node), 1, disk);

    get_modestr(modstr, node.mode);
    time = ctime(&node.modify_time);

    if (check_type(node.mode, TYPE_DIR) == FS_IS_DIR) {
      printf("%s  %-s \e[1;34m%6ld\e[0m %.12s \e[1;34m%-s\e[0m\n",
             modstr, all_users.users[node.user_id].user_name, node.size / sizeof(directory), time + 4, current_dir_content[pos].name);
    }
    else {
      printf("%s  %-s %6d %.12s %-s\n",
             modstr, all_users.users[node.user_id].user_name, node.size, time + 4, current_dir_content[pos].name);
    }
  }

  return FS_OK;
}

/*
 * file_open - Open a file, read its contains into buffer tmp file
 */
int file_open(int ino, char *name) {
  int open_inode;
  int bno, pos;
  inode node;
  char block[BLOCKSIZE];
  FILE *buf_fp = fopen(BUFFERFILE, "w+");

  /* Check if the directory or file exists */
  for (open_inode = 0; open_inode < current_dir_num; open_inode++) {
    if (strcmp(current_dir_content[open_inode].name, name) == 0)
      break;
  }
  if (open_inode == current_dir_num)
    return FS_NO_EXIST;

  open_inode = current_dir_content[open_inode].inode_id;

  /* Check mode */
  if (check_mode(open_inode, CAN_READ) == 0)
    return FS_NO_PRIVILAGE;

  /* Read inode information */
  fseek(disk, INODEPOS + open_inode * INODESIZE, SEEK_SET);
  fread(&node, sizeof(node), 1, disk);

  /* Check type */
  if (check_type(node.mode, TYPE_DIR) == FS_IS_DIR)
    return FS_IS_DIR;

  if (node.size == 0) {
    fclose(buf_fp);
    return FS_OK;
  }

  /* Read data from disk */
  for (pos = 0; pos < node.block_used_num - 1; pos++) {
    memset(block, 0, BLOCKSIZE);
    bno = node.block_used[pos];
    fseek(disk, BLOCKPOS + bno * BLOCKSIZE, SEEK_SET);
    fread(block, sizeof(char), BLOCKSIZE, disk);
    fwrite(block, sizeof(char), BLOCKSIZE, buf_fp);
    block_free(bno);
    node.size -= BLOCKSIZE;
  }
  bno = node.block_used[pos];
  fseek(disk, BLOCKPOS + bno * BLOCKSIZE, SEEK_SET);
  fread(block, sizeof(char), node.size, disk);
  fwrite(block, sizeof(char), node.size, buf_fp);
  block_free(bno);
  node.size = 0;
  node.block_used_num = 0;

  /* Save inode */
  fseek(disk, INODEPOS + open_inode * INODESIZE, SEEK_SET);
  fwrite(&node, sizeof(inode), 1, disk);

  fclose(buf_fp);
  return FS_OK;
}

/*
 * file_close - Close a file, write buffer file contents to disk
 */
int file_close(int ino, char *name) {
  int close_inode;
  int bno, read_num;
  inode node;
  char block[BLOCKSIZE];
  FILE *buf_fp = fopen(BUFFERFILE, "r");

  /* Check if the directory or file exists */
  for (close_inode = 0; close_inode < current_dir_num; close_inode++) {
    if (strcmp(current_dir_content[close_inode].name, name) == 0)
      break;
  }
  if (close_inode == current_dir_num)
    return FS_NO_EXIST;

  close_inode = current_dir_content[close_inode].inode_id;

  /* Read inode information */
  fseek(disk, INODEPOS + close_inode * INODESIZE, SEEK_SET);
  fread(&node, sizeof(node), 1, disk);

  /* Check type */
  if (check_type(node.mode, TYPE_DIR) == FS_IS_DIR)
    return FS_ISNOT_FILE;
  
  /* Read data from buffer file */
  memset(block, 0, BLOCKSIZE);
  read_num = fread(block, sizeof(char), BLOCKSIZE, buf_fp);
  while(read_num != 0) {
    bno = block_alloc();
    if (bno == FS_NO_BLOCK)
      break;
    fseek(disk, BLOCKPOS + bno * BLOCKSIZE, SEEK_SET);
    fwrite(block, sizeof(char), BLOCKSIZE, disk);
    node.block_used[node.block_used_num] = bno;
    node.block_used_num++;
    node.size += read_num;

    memset(block, 0, BLOCKSIZE);
    read_num = fread(block, sizeof(char), BLOCKSIZE, buf_fp);
  }

  /* Save inode */
  fseek(disk, INODEPOS + close_inode * INODESIZE, SEEK_SET);
  fwrite(&node, sizeof(inode), 1, disk);

  fclose(buf_fp);
  return FS_OK;
}

/*
 * file_cat - Get a file's content
 */
int file_cat(void) {
  int read_num;
  FILE *buf_fp = fopen(BUFFERFILE, "r");
  char block[BLOCKSIZE];
  memset(block, 0, BLOCKSIZE);

  /* Read data from buffer file */
  while((read_num = fread(block, sizeof(char), BLOCKSIZE, buf_fp) != 0))
    printf("%s", block);

  fclose(buf_fp);
  return FS_OK;
}

/*
 * file_mv - Move file from srcname to dstname
 */
int file_mv(int ino, char *srcname, char *dstname) {
  int pos;
  int src_inode;
  inode node;

  /* Check if the directory or file exists */
  for (pos = 0; pos < current_dir_num; pos++) {
    if (strcmp(current_dir_content[pos].name, srcname) == 0)
      break;
  }
  if (pos == current_dir_num)
    return FS_NO_EXIST;

  src_inode = current_dir_content[pos].inode_id;

  /* Check mode */
  if (check_mode(current_inode_id, CAN_READ) == 0)
    return FS_NO_PRIVILAGE;

  /* Read inode information */
  fseek(disk, INODEPOS + src_inode * INODESIZE, SEEK_SET);
  fread(&node, sizeof(node), 1, disk);

  /* Check type */
  if (check_type(node.mode, TYPE_DIR) == FS_IS_DIR)
    return FS_IS_DIR;
  
  /* Move to other directory */
  if (dstname[strlen(dstname) - 1] == '/') {
    dstname[strlen(dstname) - 1] = '\0';
    int dstpos, dst_node;
    inode dstnode;

    /* Check if the directory or file exists */
    for (dstpos = 0; dstpos < current_dir_num; dstpos++) {
      if (strcmp(current_dir_content[dstpos].name, dstname) == 0)
        break;
    }
    if (dstpos == current_dir_num)
      return FS_NO_EXIST;

    dst_node = current_dir_content[dstpos].inode_id;

    /* Check mode */
    if (check_mode(dst_node, CAN_WRITE) == 0)
      return FS_NO_PRIVILAGE;

    /* Read inode information */
    fseek(disk, INODEPOS + dst_node * INODESIZE, SEEK_SET);
    fread(&dstnode, sizeof(dstnode), 1, disk);

    /* Check type */
    if (check_type(dstnode.mode, TYPE_FILE) == FS_IS_FILE)
      return FS_IS_FILE;
    
    /* Check same name */
    dir_cd(current_inode_id, dstname);
    if (check_name(srcname) == FS_FILE_EXIST) {
      dir_close(current_inode_id);
      current_inode_id = ino;
      dir_open(ino);
      return FS_FILE_EXIST;
    }
    dir_close(current_inode_id);
    current_inode_id = ino;
    dir_open(ino);

    /* Delete entry */
    for (; pos < current_dir_num - 1; pos++) {
      current_dir_content[pos] = current_dir_content[pos + 1];
    }
    current_dir_num--;

    /* Free last block if need */
    if (current_dir_num / DIRMAXINBLK != (current_dir_num - 1) / DIRMAXINBLK) {
      current_inode.block_used_num--;
      block_free(current_inode.block_used[current_inode.block_used_num]);
    }

    /* Create new entry */
    dir_cd(current_inode_id, dstname);
    dir_creat(current_inode_id, TYPE_FILE, srcname);

    /* Copy inode */
    for (dstpos = 0; dstpos < current_dir_num; dstpos++) {
      if (strcmp(current_dir_content[dstpos].name, srcname) == 0)
        break;
    }
    dst_node = current_dir_content[dstpos].inode_id;
    fseek(disk, INODEPOS + dst_node * INODESIZE, SEEK_SET);
    fwrite(&node, sizeof(node), 1, disk);

    /* Return to src directory */
    dir_close(current_inode_id);
    current_inode_id = ino;
    dir_open(ino);
  }
  /* Move to current directory */
  else {
    /* Check mode */
    if (check_mode(current_inode_id, CAN_WRITE) == 0)
      return FS_NO_PRIVILAGE;
    
    /* Check same name */
    if (check_name(dstname) == FS_FILE_EXIST)
      return FS_FILE_EXIST;

    strcpy(current_dir_content[pos].name, dstname);
    /* Save data */
    dir_close(current_inode_id);
    dir_open(ino);
  }

  return FS_OK;
}

/*
 * file_cp - Copy file from srcname to dstname
 */
int file_cp(int ino, char *srcname, char *dstname) {
  int pos, bno;
  int src_inode;
  inode node;
  char block[BLOCKSIZE];
  FILE *buf_fp = fopen(BUFFERFILE, "w+");

  /* Check if the directory or file exists */
  for (pos = 0; pos < current_dir_num; pos++) {
    if (strcmp(current_dir_content[pos].name, srcname) == 0)
      break;
  }
  if (pos == current_dir_num)
    return FS_NO_EXIST;

  src_inode = current_dir_content[pos].inode_id;

  /* Check mode */
  if (check_mode(current_inode_id, CAN_READ) == 0)
    return FS_NO_PRIVILAGE;

  /* Read inode information */
  fseek(disk, INODEPOS + src_inode * INODESIZE, SEEK_SET);
  fread(&node, sizeof(node), 1, disk);

  /* Check type */
  if (check_type(node.mode, TYPE_DIR) == FS_IS_DIR)
    return FS_IS_DIR;
  
  /* Copy file content */
  if (node.size == 0) {
    fclose(buf_fp);
    return FS_OK;
  }
  for (pos = 0; pos < node.block_used_num - 1; pos++) {
    memset(block, 0, BLOCKSIZE);
    bno = node.block_used[pos];
    fseek(disk, BLOCKPOS + bno * BLOCKSIZE, SEEK_SET);
    fread(block, sizeof(char), BLOCKSIZE, disk);
    fwrite(block, sizeof(char), BLOCKSIZE, buf_fp);
  }
  bno = node.block_used[pos];
  fseek(disk, BLOCKPOS + bno * BLOCKSIZE, SEEK_SET);
  fread(block, sizeof(char), node.size, disk);
  fwrite(block, sizeof(char), node.size, buf_fp);
  fclose(buf_fp);
  
  /* Copy to other directory */
  if (dstname[strlen(dstname) - 1] == '/') {
    dstname[strlen(dstname) - 1] = '\0';
    int dstpos, dst_node;
    inode dstnode;

    /* Check if the directory or file exists */
    for (dstpos = 0; dstpos < current_dir_num; dstpos++) {
      if (strcmp(current_dir_content[dstpos].name, dstname) == 0)
        break;
    }
    if (dstpos == current_dir_num)
      return FS_NO_EXIST;

    dst_node = current_dir_content[dstpos].inode_id;

    /* Check mode */
    if (check_mode(dst_node, CAN_WRITE) == 0)
      return FS_NO_PRIVILAGE;

    /* Read inode information */
    fseek(disk, INODEPOS + dst_node * INODESIZE, SEEK_SET);
    fread(&dstnode, sizeof(dstnode), 1, disk);

    /* Check type */
    if (check_type(dstnode.mode, TYPE_FILE) == FS_IS_FILE)
      return FS_IS_FILE;
    
    /* Check same name */
    dir_cd(current_inode_id, dstname);
    if (check_name(srcname) == FS_FILE_EXIST) {
      dir_close(current_inode_id);
      current_inode_id = ino;
      dir_open(ino);
      return FS_FILE_EXIST;
    }
    dir_close(current_inode_id);
    current_inode_id = ino;
    dir_open(ino);

    /* Create new entry */
    dir_cd(current_inode_id, dstname);
    dir_creat(current_inode_id, TYPE_FILE, srcname);

    /* Save file content */
    file_close(current_inode_id, srcname);

    /* Return to src directory */
    dir_close(current_inode_id);
    current_inode_id = ino;
    dir_open(ino);
  }
  /* Copy to current directory */
  else {
    /* Check mode */
    if (check_mode(current_inode_id, CAN_WRITE) == 0)
      return FS_NO_PRIVILAGE;
    
    /* Check same name */
    if (check_name(dstname) == FS_FILE_EXIST)
      return FS_FILE_EXIST;

    /* Create new file */
    dir_creat(ino, TYPE_FILE, dstname);
    
    /* Save file content */
    file_close(ino, dstname);
  }

  return FS_OK;
}


/********************ASSIST FUNCTIONS********************/


/*
 * oct2dec - Change Octal number to decimal number
 * 
 * For this program use, only supply 0~7777
 */
int oct2dec(int oct_number) {
  int dec_number = 0;
  int a, b, c, d;
  a = oct_number / 1000;
  b = (oct_number % 1000) / 100;
  c = (oct_number % 100) / 10;
  d = oct_number % 10;
  dec_number = ((a * 8 + b) * 8 + c) * 8 + d;
  return dec_number;
}

/*
 * check_name - Check if the file already exists
 */
int check_name(char *name) {
  int i;
  for (i = 0; i < current_dir_num; i++) {
    if (strcmp(name, current_dir_content[i].name) == 0)
      return FS_FILE_EXIST;
  }
  return FS_OK;
}

/*
 * check_type - Check file type
 */
int check_type(int mode, int type) {
  int isdir = mode & (1 << 9);
  if (isdir == (1 << 9) && type == TYPE_FILE)
    return FS_ISNOT_FILE;
  else if (isdir == 0 && type == TYPE_DIR)
    return FS_ISNOT_DIR;
  else if (isdir == 0 && type == TYPE_FILE)
    return FS_IS_FILE;
  else
    return FS_IS_DIR;
}

/*
 * check_mode - Check if user have privilege to do operation
 */
int check_mode(int ino, int operation) {
  inode node;
  int ret;

  fseek(disk, INODEPOS + ino * INODESIZE, SEEK_SET);
  fread(&node, sizeof(node), 1, disk);

  if (operation == CAN_READ) {
    if (node.user_id == current_user_id)
      ret = ((node.mode & (1 << 8)) == (1 << 8));
    else
      ret = ((node.mode & (1 << 2)) == (1 << 2));
  }
  else {
    if (node.user_id == current_user_id)
      ret = ((node.mode & (1 << 7)) == (1 << 7));
    else
      ret = ((node.mode & (1 << 1)) == (1 << 1));
  }
  return ret;
}

/*
 * path_cd - Change path when enter a directory
 */
void path_change(int old_inode_id, char *name) {
  int pos;
  if (!strcmp(name, ".") || (!strcmp(name, "..") && (old_inode_id == 0)))
    return;
  else if (!strcmp(name, "..") && current_inode_id != 0) {
    for (pos = strlen(path) - 1; pos >= 0; pos--) {
      if (path[pos] == '/') {
        path[pos] = '\0';
        strcat(path, " >");
        break;
      }
    }
  }
  else if (!strcmp(name, "..") && current_inode_id == 0) {
    for (pos = strlen(path) - 1; pos >= 0; pos--) {
      if (path[pos] == '/') {
        path[pos + 1] = '\0';
        strcat(path, " >");
        break;
      }
    }
  }
  else if (path[strlen(path) - 3] == '/') {
    path[strlen(path) - 2] = '\0';
    strcat(path, name);
    strcat(path, " >");
  }
  else {
    path[strlen(path) - 2] = '\0';
    strcat(path, "/");
    strcat(path, name);
    strcat(path, " >");
  }
}

/*
 * mtime_change - Change modified time for a file
 */
int mtime_change(int ino, char *name) {
  int i;
  int ch_node;
  inode node;

  /* Check if the directory or file exists */
  for (i = 0; i < current_dir_num; i++) {
    if (strcmp(current_dir_content[i].name, name) == 0)
      break;
  }
  ch_node = current_dir_content[i].inode_id;

  /* Check if this is a directory */
  fseek(disk, INODEPOS + ch_node * INODESIZE, SEEK_SET);
  fread(&node, sizeof(node), 1, disk);
  
  /* Update modify time */
  time_t timer;
	time(&timer);
  node.modify_time = timer;

  /* Save node data */
  fseek(disk, INODEPOS + ch_node * INODESIZE, SEEK_SET);
  fwrite(&node, sizeof(node), 1, disk);

  return FS_OK;
}

/*
 * path_cd - Change path when enter a directory
 */
void get_modestr(char *modstr, int mode) {
  strcpy(modstr, "----------");
  if ((mode & (1 << 9)) == (1 << 9))
    modstr[0] = 'd';
  if ((mode & (1 << 8)) == (1 << 8))
    modstr[1] = 'r';
  if ((mode & (1 << 7)) == (1 << 7))
    modstr[2] = 'w';
  if ((mode & (1 << 6)) == (1 << 6))
    modstr[3] = 'x';
  if ((mode & (1 << 5)) == (1 << 5))
    modstr[4] = 'r';
  if ((mode & (1 << 4)) == (1 << 4))
    modstr[5] = 'w';
  if ((mode & (1 << 3)) == (1 << 3))
    modstr[6] = 'x';
  if ((mode & (1 << 2)) == (1 << 2))
    modstr[7] = 'r';
  if ((mode & (1 << 1)) == (1 << 1))
    modstr[8] = 'w';
  if ((mode & 1) == 1)
    modstr[9] = 'x';
}

/*
 * mode_change - Change a file's mode
 */
int mode_change(int mode, char *name) {
  int i;
  int change_inode;
  inode node;

  /* Invalid mode */
  if (mode < 0 || mode > 777)
    return FS_INVALID_MODE;

  /* Check if the directory or file exists */
  for (i = 0; i < current_dir_num; i++) {
    if (strcmp(current_dir_content[i].name, name) == 0)
      break;
  }
  if (i == current_dir_num)
    return FS_NO_EXIST;
  change_inode = current_dir_content[i].inode_id;

  /* Check if this is a directory */
  fseek(disk, INODEPOS + change_inode * INODESIZE, SEEK_SET);
  fread(&node, sizeof(node), 1, disk);

  /* Only owner can change the mode */
  if (node.user_id != current_user_id)
    return FS_NO_PRIVILAGE;

  mode = oct2dec(mode);
  mode |= (node.mode & (1 << 9));

  node.mode = mode;

  /* Save node data */
  fseek(disk, INODEPOS + change_inode * INODESIZE, SEEK_SET);
  fwrite(&node, sizeof(node), 1, disk);

  return FS_OK;
}

int check_if_readonly(int ino, char *name) {
  int pid, status;
  int check_inode;
  int bno, pos;
  inode node;
  char block[BLOCKSIZE];
	char *vim_arg[] = {"vim", BUFFERFILE, NULL};
  FILE *buf_fp = fopen(BUFFERFILE, "w+");

  /* Check if the directory or file exists */
  for (check_inode = 0; check_inode < current_dir_num; check_inode++) {
    if (strcmp(current_dir_content[check_inode].name, name) == 0)
      break;
  }
  if (check_inode == current_dir_num)
    return FALSE;

  check_inode = current_dir_content[check_inode].inode_id;

  /* Check mode */
  if (check_mode(check_inode, CAN_READ) == 0 || check_mode(check_inode, CAN_WRITE) == 1)
    return FALSE;
  
  /* Read only */

  /* Read inode information */
  fseek(disk, INODEPOS + check_inode * INODESIZE, SEEK_SET);
  fread(&node, sizeof(node), 1, disk);

  /* Check type */
  if (check_type(node.mode, TYPE_DIR) == FS_IS_DIR)
    return FALSE;

  /* Read data from disk */
  for (pos = 0; pos < node.block_used_num - 1; pos++) {
    memset(block, 0, BLOCKSIZE);
    bno = node.block_used[pos];
    fseek(disk, BLOCKPOS + bno * BLOCKSIZE, SEEK_SET);
    fread(block, sizeof(char), BLOCKSIZE, disk);
    fwrite(block, sizeof(char), BLOCKSIZE, buf_fp);
  }
  bno = node.block_used[pos];
  fseek(disk, BLOCKPOS + bno * BLOCKSIZE, SEEK_SET);
  fread(block, sizeof(char), node.size, disk);
  fwrite(block, sizeof(char), node.size, buf_fp);

  fclose(buf_fp);

  if ((pid = fork()) == 0) {
		execvp("vim", vim_arg);
	}

  wait(&status);
  return TRUE;
}
