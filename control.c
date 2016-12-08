#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]){
  int errno;
  int semid;
  int key = ftok("makefile" , 22);
  int sc;
  char *file = "ring";

  if (strncmp(argv[1], "-c", strlen(argv[1])) == 0){
    semid = semget(key, 1, IPC_CREAT | IPC_EXCL | O_APPEND | 0644);
    printf("semaphore created: %d\n", semid);
    union semun su;
    printf("value set: %d\n", sc);
    /*    su.val = 0;
    //setting semaphore value
    sc = semctl(semid, 0, SETVAL, su);
    printf("value set: %d\n", sc);*/
    if(open(file, O_CREAT | O_TRUNC | O_WRONLY, 0644) < 0) {
      printf("Could not create/open file: %s", strerror(errno));
    }
  }

  else if(strncmp(argv[1], "-r", strlen(argv[1])) == 0){
    semid = semget(key, 1, 0);
    //removing a semaphore
    sc = semctl(semid, 0, IPC_RMID);
    printf("semaphore removed: %d\n", sc);
    char ret [sizeof(file)];
    fgets(ret, strlen(ret) -1, file);
    printf("The Story:\n%s", ret);
  }
  return 0;

}
