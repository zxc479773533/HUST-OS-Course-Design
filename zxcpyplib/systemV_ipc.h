/*
 * zxcpyp's lib
 *
 * zxcpyp system V IPC header
 */

#ifndef ZXCPYP_SYSTRMV_IPC_H
#define ZXCPYP_SYSTRMV_IPC_H

#include <sys/types.h>  /* System data type */
//#include <sys/msg.h>    /* System V Message */
#include <sys/sem.h>    /* System V Semaphore */
//#include <sys/shm.h>    /* System V Shared Memory */
#include <errno.h>      /* Error difinations */

/*
+---------------+---------------+---------------+---------------+
|      IPC      |    Message    |   Semaphore   | Shared Memory |
+---------------+---------------+---------------+---------------+
|   Head file   |  <sys/msg.h>  |  <sys/sem.h>  |  <sys/shm.h>  |
+---------------+---------------+---------------+---------------+
| Data structure|    msqid_ds   |    semid_ds   |    shmid_ds   |
+---------------+---------------+---------------+---------------+
| Create Object |    msgget()   |    semget()   |shmget()shmat()|
+---------------+---------------+---------------+---------------+
|    Control    |    msgctl()   |    semctl()   |    shmctl()   |
+---------------+---------------+---------------+---------------+
|      IPC      |    msgsnd()   |    semop()    |               |
|    Operate    |    msgrcv()   |     (P/V)     |               |
+---------------+---------------+---------------+---------------+
*/

/* True and false defines */
#ifndef FALSE
  #define FALSE 0
#endif // !FALSE
#ifndef TRUE
  #define TRUE 1
#endif // !TRUE

#endif // !ZXCPYP_SYSTRMV_IPC_H
