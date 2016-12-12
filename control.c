#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

/*
union semun {
           int val;    // Value for SETVAL 
           struct semid_ds *buf;    // Buffer for IPC_STAT, IPC_SET
           unsigned short *array;  // Array for GETALL, SETALL
           struct seminfo *__buf;  // Buffer for IPC_INFO
};
*/

int createSem(int key, int val) {
  int semid;
  semid = semget(key, 1, IPC_CREAT | IPC_EXCL | O_APPEND | 0644);
  if (semid > 0) { 
    printf("semaphore created: %d\n", semid);
    union semun su;
    su.val = val;
    printf("value set: %d\n", su.val);
    return semid;
  }
  else { return -1; }
}

int getSem(int key) {
  return semget(key, 1, 0);
}

int getSemVal(int key) {
  return semctl(key, 0, GETVAL);
}

void remSem(int key) {
  semctl(key, 0, IPC_RMID);
}

char * readFile(char *file) {
  struct stat f;
  if (stat(file, &f) < 0) {
    printf("Stat error\n");
    return NULL;
  }

  int fd = open(file, O_RDONLY);
  int size = f.st_size;
  char * buff = (char *) malloc(size + 1);
  read(fd, buff, size);
  close(fd);
  return buff;
}

int main(int argc, char *argv[]){
  int errno;

  int semkey = ftok("makefile", 22); //key for semaphore
  int smkey = ftok("makefile", 21); //key for shared memory

  char *file = "ring";

  if (argv[1] == NULL) {
    printf("No argument given\n");
    return -1;;
  }

  if (strncmp(argv[1], "-c", strlen(argv[1])) == 0) {
    //Create/open file
    umask(0000);
    int fd = open(file, O_CREAT | O_TRUNC | O_WRONLY, 0644);      
    if(fd < 0) {
      printf("Could not create/open file: %s", strerror(errno));
      return 1;
    }
    close(fd);
    //Create shared memory
    int Shm = shmget(smkey,sizeof(int), IPC_CREAT | 0644);
    if (Shm < 0) {
      printf("Could not create shared memory: %s\n", strerror(errno));
      return 1;
    }
    printf("Shared memory created: %d\n", Shm);
    //Create semaphore
    int semid = createSem(semkey, 1);
    if (semid < 0) {
      printf("Could not create semaphore: %s\n", strerror(errno));
      return 1;
    }
  }

  else if(strncmp(argv[1], "-r", strlen(argv[1])) == 0){
    //Remove shared memory
    int Shm = shmget(smkey, 0, 0);
    if (Shm < 0) {
      printf("Could not access shared memory: %s\n", strerror(errno));
    }
    else {
      shmctl(Shm, IPC_RMID, 0);
      printf("Removed shared memory: %d\n", Shm);
    }
    //Remove semaphore
    int semid = getSem(semkey);
    if (semid < 0) {
      printf("Could not access semaphore: %s\n", strerror(errno));
    }
    remSem(semid);
    printf("Semaphore removed: %d\n", semid);
    //Print out file contents
    printf("File contents:\n%s", readFile(file));
  }

  else if (strncmp(argv[1], "-v", strlen(argv[1])) == 0){
    printf("File contents:\n%s", readFile(file));
  }

  else {
    printf("Incorrect argument given\n");
    return -1;;
  }

  return 1;
}

