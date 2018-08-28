/*
 * HUST OS Design - Part V
 * 
 * File system simulation - Header
 * 
 * Created by zxcpyp at 2018-08-26
 * 
 * Github: zxc479773533
 */

#ifndef ZXCPYP_FILESYSTEM
#define ZXCPYP_FILESYSTEM

#include "../lib/zxcpyp_sys.h"
#include "fs_error.h"
#include <time.h>

/* Disk parameters */
#define BLOCKSIZE 1024          /* The block size */
#define INODENUM 1024           /* The Inode nums */
#define BLOCKNUM (1024 * 64)    /* The disk size 64 MB */
#define FILEBLKMAX (1024 * 4)   /* The file max size is 4 MB */
#define FILENAMEMAX 20           /* File and directory name max length */
#define USERNAMEMAX 20          /* User name max length */
#define USERPWDMAX 20           /* User password max length */
#define USERNUMMAX 10

#define INODESIZE sizeof(inode)                         /* The inode size */
#define SUPERPOS sizeof(sys_users)                      /* The #1 is super block */
#define INODEPOS (SUPERPOS + sizeof(super_block))       /* The #2 ~ #1025 are inodes */
#define BLOCKPOS (INODEPOS + INODESIZE * INODENUM)      /* The #1026 is first free block */


#define TYPE_DIR 0
#define TYPE_FILE 1

#define DIRMAXINBLK (BLOCKSIZE / sizeof(directory)) /* Directory entry num per block */
#define DIRMAXNUM (FILEBLKMAX * DIRMAXINBLK)        /* File num per directory */

/* File system data structures */
typedef struct super_block {
  int inode_map[INODENUM];
  int block_map[BLOCKNUM];
  int inode_free_num;
  int block_free_num;
} super_block;

typedef struct inode {
  int block_used[FILEBLKMAX];
  int block_used_num;
  int size;
  int mode;
  int creat_time;
  int modify_time;
  int user_id;
} inode;

typedef struct directory {
  char name[FILENAMEMAX];
  int inode_id;
} directory;

typedef struct user {
  char user_name[USERNAMEMAX];
  char user_pwd[USERPWDMAX];
} user;

typedef struct sys_users {
  int user_map[USERNUMMAX];
  int user_num;
  user users[USERNUMMAX];
} sys_users;

/* Global variables */
extern FILE *disk;
extern super_block super;
extern int current_inode_id;
extern inode current_inode;
extern directory current_dir_content[DIRMAXNUM];
extern int current_dir_num;
extern int current_user_id;

extern char path[128];

/* Developer functions */
void reset_disk();
void print_current_inode_id(void);
void print_current_dir_num(void);
void show_files_info();
void print_superblk_inode_info(int pos);
void print_superblk_block_info(int pos);

/* Core function */
int inode_alloc(void);
int inode_free(int ino);
int init_dir_inode(int new_ino, int ino);
int init_file_inode(int new_ino);
int block_alloc(void);
int block_free(int bno);

/* User function */
int login(void);
int user_add(char *name, char *pwd);
int user_pwd(char *name, char *new_pwd);
int user_del(char *name);

/* Disk function */
int load_super_block(void);
int format_disk(void);
int close_disk(void);

/* File and directory function */
int dir_open(int ino);
int dir_close(int ino);
int dir_creat(int ino, int type, char *name);
int dir_rm(int ino, int type, char *name);
int dir_cd(int ino, char *path);
int dir_ls(void);

/* Assist function */
int oct2dec(int oct_number);
int check_name(char *name);
int check_type(int mode, int type);
void path_change(int old_inode_id, char *name);
int mtime_change(int ino, char *name);

#endif // !ZXCPYP_FILESYSTEM
