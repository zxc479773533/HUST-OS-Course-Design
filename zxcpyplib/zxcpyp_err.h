/*
 * zxcpyp's lib
 *
 * zxcpyp error message header
 */

#ifndef ZXCPYP_ERR_H
#define ZXCPYP_ERR_H

#include <stdio.h>
#include <stdlib.h>

/*
 * err_msg - Print error message
 */
void err_msg(const char *err);

/*
 * err_exit - Announce error and exit
 */
void err_exit(const char *err);

/*
 * usage_err - Announce usage and exit
 */
void usage_err(const char *err);

/*
 * fatal_err - Handle fatal error
 */
void fatal_err(const char *err);

#endif // !ZXCPYP_ERR_H