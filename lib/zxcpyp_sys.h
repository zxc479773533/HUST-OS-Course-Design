/*
 * zxcpyp's lib
 *
 * zxcpyp sys header
 */

#ifndef ZXCPYP_H
#define ZXCPYP_H

#include <stdio.h>      /* Standard I/O functions */
#include <stdlib.h>     /* Standard library */
#include <string.h>     /* String handling functions */
#include <unistd.h>     /* Unix system calls */
#include <errno.h>      /* Error difinations */
#include <fcntl.h>      /* Micros for I/O */
#include <sys/types.h>  /* System data type */
#include <sys/stat.h>   /* File system status */

#include "zxcpyp_err.h" /* Error handling functions */

/* True and false defines */
#ifndef FALSE
  #define FALSE 0
#endif // !FALSE
#ifndef TRUE
  #define TRUE 1
#endif // !TRUE

/* Max and min defines */
#ifndef min
  #define min(m, n) ((m) < (n) ? (m) : (n))
#endif // !min
#ifndef max
  #define max(m, n) ((m) > (n) ? (m) : (n))
#endif // !max

#endif // !ZXCPYP_H
