/*
 * HUST OS Design - Part III
 * 
 * Device driver - Character device driver
 * 
 * Created by zxcpyp at 2018-07-28
 * 
 * Github: zxc479773533
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

/* Cmd for ioctl */
#define MEM_CLEAR 1

/*
 * print_usage - Print usage of testdev
 */
void print_usage(void) {
  printf("Usage: \n");
  printf("\t./testdev read <startpos> <readnum>\n");
  printf("\t./testdev write <startpos> <string>\n");
  printf("\t./testdev ioctl clear\n");
}

int main(int argc, char **argv) {
  printf("/*\n");
  printf(" * HUST OS Design - Part III\n");
  printf(" *\n");
  printf(" * Test Device: zxcpypdriver\n");
  printf(" */\n\n");
  int fd, start, num;
  char buf[1024];
  fd = open("/dev/zxcpypdriver", O_RDWR);
  if (fd < 0) {
    printf("Open error!\n");
    return 0;
  }
  if (argc == 4 && strncmp(argv[1], "read", 4) == 0) {
    start = atoi(argv[2]);
    num = atoi(argv[3]);
    lseek(fd, start, SEEK_SET);
    read(fd, buf, num);
    printf("Read: %s\n", buf);
  }
  else if (argc == 4 && strncmp(argv[1], "write", 5) == 0) {
    start = atoi(argv[2]);
    lseek(fd, start, SEEK_CUR);
    write(fd, argv[3], strlen(argv[3]));
    printf("Write succeed!\n");
  }
  else if (argc == 3 && strncmp(argv[1], "ioctl", 5) == 0) {
    if (strncmp(argv[3], "clear", 5) == 0) {
      ioctl(fd, MEM_CLEAR, NULL);
      printf("Clear success!\n");
    }
    else
      print_usage();
  }
  else
    print_usage();

  close(fd);
  return 0;
}
