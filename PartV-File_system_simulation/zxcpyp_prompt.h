/*
 * zxcpyp prompt - A interactive shell template
 * 
 * Created by zxcpyp at 2018-02-22
 * 
 * Github: zxc479773533
 * 
 * Used for: HUST OS Design - Part V in 2018-08-26
 */

#include "zxcpyp_exec.h"

/* Constants */
#define MAXLINE 1024
#define MAXARGS 128

/* Global variables */

// char prompt[] = "zxcpyp > ";
char theme[] = "Here to control zxcpyp's file system";

/* Founction prototypes */
void eval(char *cmdline);
int parseline(const char *cmdline, char **argv);

/* Print file system version */
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

/* Print usage messages */
void print_usage(void) {
  printf("Usage: ./zxcpypfs [options]\n");
  printf("Options:\n");
  printf("\t-h: print this massges\n");
  printf("\t-p: hide the prompt\n");
  printf("\t-d: use developer mode\n");
  printf("\tdefault: start shell\n");
}

int Start_Shell(int argc, char **argv) {
  int ret;
  char ch;
  /* The command line */
  char cmdline[MAXLINE];
  /* Decide whether print a prompt, default yes*/
  int emit_prompt = 1;

  /* Parse the command line */
  while ((ch = getopt(argc, argv, "hpd")) != EOF) {
    switch(ch) {
    case 'h':
      print_usage();
      exit(0);
    case 'p':
      emit_prompt = 0;
      break;
    case 'd':
      developer = 0;
      break;
    default:
      print_usage();
      exit(1);
    }
  }

  /* Login */
  print_version();
  while (login() != FS_LOGIN) {}
  ret = load_super_block();
  if (ret == FS_RD_ERROR)
    return FS_RD_ERROR;

  /* Print informations */
  printf("Wherecome to zxcpyp's interactive shell!\n");
  printf("%s\n", theme);
  printf("\n");

  /* Excute the shell's read/eval loop */
  while (1) {

    /* Print prompt */
    if (emit_prompt) {
      if (developer == 0)
        printf("\e[1;31m[Developer mode] \e[0m");
      printf("\e[1;32m%s \e[0m", path);
      fflush(stdout);
    }
    if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin)) {
      printf("fgets error\n");
      exit(1);
    }
    if (feof(stdin)) {
      printf("[EXIT] Parser reached end-of-file. Terminated!\n");
      fflush(stdout);
      exit(0);
    }

    /* Evaluate the command line */
    eval(cmdline);
    fflush(stdout);
  }

  return 0;
}

/*
 * eval - Evaluate the command line that the user has just typed in
 * 
 * If the user has requseted a built-in command, then excute it immediately.
 */
void eval(char *cmdline) {
  char *argv[MAXARGS];
  char buf[MAXLINE];
  int argc;

  /* Parse command */
  strcpy(buf, cmdline);
  argc = parseline(buf, argv);

  if (argv[0] == NULL)
    return;
  if (!developer_cmd(argc, argv)) {
    if (!builtin_cmd(argv)) {
      if (!py_execute(argv[0], argc, argv))
        printf("zxcpypsh: command not found: %s\n", argv[0]);
      // else
        // printf("[INFO] Operation finished.\n\n");
    }
  }
}

/*
 * parseline - Parse the command line and build the argv array.
 */
int parseline(const char *cmdline, char **argv) {
  /* Holds local copy of command line */
  static char array[MAXLINE];
  char *buf = array;
  char *delim;
  int argc;
  
  strcpy(buf, cmdline);
  buf[strlen(buf) - 1] = ' ';
  /* Ignore leading spaces */
  while (*buf && (*buf == ' '))
    buf++;

  /* Build the argv list */
  argc = 0;
  if (*buf == '\'') {
    buf++;
    delim = strchr(buf, '\'');
  }
  else {
    delim = strchr(buf, ' ');
  }

  while(delim) {
    argv[argc++] = buf;
    *delim = '\0';
    buf = delim + 1;
    while (*buf && (*buf == ' '))
      buf++;
    
    if (*buf == '\'') {
      buf++;
      delim = strchr(buf, '\'');
    }
    else {
      delim = strchr(buf, ' ');
    }
  }
  argv[argc] = NULL;
  return argc;
}
