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
    printf("pos: %d\n", pos);
    printf("name: %s\n", current_dir_content[pos].name);
    printf("inode id: %d\n", current_dir_content[pos].inode_id);
    printf("user id: %d\n\n", node.user_id);
  }
  printf("\n");
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
  node.modify_time = 0;
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
  node.modify_time = 0;
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
    if (strcmp(uname, all_users.users[i].user_name) == 0) {
      if (strcmp(upwd, all_users.users[i].user_pwd) == 0) {
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
  current_inode.modify_time = 0;
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
  if (node.user_id != current_user_id)
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
int dir_cd(int ino, char *path) {
  int i;
  int cd_inode;
  inode node;

  /* Check if the directory or file exists */
  for (i = 0; i < current_dir_num; i++) {
    if (strcmp(path, current_dir_content[i].name) == 0)
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

  dir_close(ino);
  current_inode_id = cd_inode;
  dir_open(cd_inode);

  return FS_OK;
}

/*
 * dir_ls - List all files in directory
 */
int dir_ls(void) {
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
  c = (oct_number % 100) / 100;
  d = oct_number % 10;
  dec_number = d + c * 8 + b * 64 + a * 512;
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
 *
 */
int mtime_change(int ino, char *name) {
  int i;
  int ch_node;
  inode node;

  /* Check if the directory or file exists */
  for (i = 0; i < current_dir_num; i++) {
    if (strcmp(path, current_dir_content[i].name) == 0)
      break;
  }
  ch_node = current_dir_content[i].inode_id;

  /* Check if this is a directory */
  fseek(disk, INODEPOS + current_dir_content[ch_node].inode_id * BLOCKSIZE, SEEK_SET);
  fread(&node, sizeof(node), 1, disk);
  
  /* Update modify time */
  time_t timer;
	time(&timer);
  node.modify_time = timer;

  return FS_OK;
}
