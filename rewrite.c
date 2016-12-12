#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(){
  int key = ftok("makefile", 9); //key for semaphore
  int semid;
  int fd; //file descriptor
  struct sembuf sb;
  sb.sem_num = 0;
  sb.sem_flg = SEM_UNDO;
  sb.sem_op = -1;


  semid = semget( key, 1, 0644);
  sb.sem_op = 1;
printf("First Message: %d\n", semid);
  semop(semid, &sb, 1);
  fd = open("ring", O_APPEND | O_RDWR | 0644);
  printf("second Message: %d\n", semid);
int sd = shmget( key, sizeof(int), 0644 );
int *size = (int *) shmat( sd, 0, 0 );   //size of last line 



  lseek(fd, -(*size), SEEK_END);
  char line[(*size) + 1];   //previous line
  int rd = read(fd, line, *size); //reading descriptor
  line[rd] = '\0';
  printf("Last Message: %s\n", line);

  char next[100];
  printf("Enter a message: ");
  fgets(next, sizeof(next), stdin);
  *size = strlen(next);
  write(fd, next, *size);
  close(fd);
  sb.sem_op = 1;
  semop(semid, &sb, 1);

  return 0;
}
