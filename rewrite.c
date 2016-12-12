#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

char *lastLine(char *file, int smkey) {
  int errno;
  int f = open(file, O_RDONLY, 0);

  if(f < 0) {
    printf("Cannot access file: %s", strerror(errno));
    return NULL;
  }
  int semid = shmget(smkey, 0, 0);
  if(semid < 0) {
    printf("Cannot access shared memory: %s", strerror(errno));
    return NULL;
  }

  int *size = shmat(semid, 0, 0);
  char *buff = malloc(*size + 1);
  if(*size) {
    lseek(f, -1, *size, SEEK_END);
    read(f, buff, *size);
    buff[strcspn(buff, "\n")] = 0;  
  }
  return buff;
}

void getNextLine(char *file, int semkey, int smkey) {
  int errno;

  int shmid = shmget(smkey, 1, 0);
  int semid = semget(semkey, 1, 0);

  if (shmid < 0) {
    printf("Cannot access shared memory: %s\n", strerror(errno));
    return;
  }
  if (semid < 0) {
    printf("Cannot access semaphore: %s\n", strerror(errno));
    return;
  }

  struct sembuf s;
  s.sem_num = 0;
  s.sem_flg = SEM_UNDO;
  s.sem_op = -1;
  semop(semid, &s, 1);

  //Get input
  char line[1000];
  fgets(line, sizeof(line), stdin);

  //Update shared memory
  int *size = shmat(semid, 0, 0);
  *size = strlen(line);
  shmdt(size);

  //Update file
  int f = open(file, O_WRONLY | O_APPEND, 0);

  if (f < 0) {
    printf("Cannot access file %s\n", strerror(errno));
  } 
  else {
    write(f, line, *size);
    close(f);
  }

  //give back sem
  s.sem_op = 1;
  semop(semid, &s, 1);
}

int main(){
  int semkey = ftok("makefile", 22);
  int smkey = ftok("makefile", 21);
  char *file = "ring";

  char * lastMess =  lastLine(file, smkey);
  printf("Last Message: %s\n", lastMess);
  free(lastMess);

  printf("Enter next message: ");
  getNextLine(file, semkey, smkey);
  return 1;
}
