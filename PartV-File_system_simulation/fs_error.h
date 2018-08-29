#ifndef ZXCPYP_FS_ERROR
#define ZXCPYP_FS_ERROR

#define FS_OK 0
#define FS_NO_EXIST -1
#define FS_RD_ERROR -2
#define FS_WR_ERROR -3
#define FS_NO_BLOCK -4
#define FS_NO_INODE -5
#define FS_DIR_FULL -6
#define FS_FILE_EXIST -7
#define FS_NO_PRIVILAGE -8
#define FS_ISNOT_FILE -9
#define FS_IS_FILE -10
#define FS_ISNOT_DIR -11
#define FS_IS_DIR -12
#define FS_DIR_NOEMPTY -13
#define FS_USER_EXIST -14
#define FS_USER_NOT_EXIST -15
#define FS_INVALID_MODE -16

#define FS_INVALID -98

#define FS_LOGIN -99
#define FS_LOGIN_ERROR -100

#endif // !ZXCPYP_FS_ERROR