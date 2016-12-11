#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]){
  int errno;
  int semid;
  int key = ftok("makefile", 22); //key for semaphore
  int sc; //semaphore value
  int fd; //file descriptor
  int *sd; //shared memory descriptor 
  char *file = "ring";
  char buffer[1000];

  if (strncmp(argv[1], "-c", strlen(argv[1])) == 0){
      sd = shmat(shmget(ftok("makefile", 9), 50, IPC_CREAT | 0644),0, 0);
      semid = semget(key, 1, IPC_CREAT | IPC_EXCL | O_APPEND | 0644);
      if (semid > 0) { 
    	printf("semaphore created: %d\n", semid);
    	union semun su;
    	printf("value set: %d\n", sc);
    	fd = open(file, O_CREAT | O_TRUNC | O_WRONLY, 0644);      
     	if(fd < 0) 
      	  printf("Could not create/open file: %s", strerror(errno));
      }
      else printf("Semaphore already exists!\n");
  }
  else if(strncmp(argv[1], "-r", strlen(argv[1])) == 0){
      semid = semget(key, 1, 0);
      //removing a semaphore
      sc = semctl(semid, 0, IPC_RMID);
      printf("semaphore removed: %d\n", sc);
      read(fd, &buffer, sizeof(buffer));
      printf("File contents:\n%s", buffer);
  }
  else if (strncmp(argv[1], "-v", strlen(argv[1])) == 0){
      read(fd, &buffer, sizeof(buffer));
      printf("File contents:\n%s", buffer);
  }
  return 0;
}
