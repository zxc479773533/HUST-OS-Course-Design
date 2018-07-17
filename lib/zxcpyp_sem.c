/*
 * zxcpyp's lib
 *
 * zxcpyp semaphore lib (System V)
 */

#include "zxcpyp_sem.h"

int UseSemUndo = FALSE;   /* Whether using SEM_UNDO in semop() */
int RetryOnEintr = TRUE;  /* Whether retry when semop() is interrupted */

int init_sem_available(int sem_id, int sem_num) {
  union semun arg;     /* semget() use arg to control */
  arg.val = 1;         /* Semphore available */
  return semctl(sem_id, sem_num, SETVAL, arg);
}

int init_sem_in_use(int sem_id, int sem_num) {
  union semun arg;     /* semget() use arg to control */
  arg.val = 0;         /* Semphore in use */
  return semctl(sem_id, sem_num, SETVAL, arg);
}

int init_sem_value(int sem_id, int sem_num, int sem_value) {
  union semun arg;     /* semget() use arg to control */
  arg.val = sem_value; /* Ordered semphore value */
  return semctl(sem_id, sem_num, SETVAL, arg);
}

int sem_p(int sem_id, int sem_num) {
  struct sembuf sops;  /* semop() use sops to control */
  sops.sem_num = sem_num;
  sops.sem_op = -1;    /* the P operation */
  sops.sem_flg = UseSemUndo ? SEM_UNDO : 0;
  while(semop(sem_id, &sops, 1) == -1)
    /* The operation was interrupted by a signal handler */
    if (errno != EINTR || !RetryOnEintr)
      return  -1;
  return 0;
}

int sem_v(int sem_id, int sem_num) {
  struct sembuf sops;  /* semop() use sops to control */
  sops.sem_num = sem_num;
  sops.sem_op = 1;     /* the V operation */
  sops.sem_flg = UseSemUndo ? SEM_UNDO : 0;
  return semop(sem_id, &sops, 1);
}
