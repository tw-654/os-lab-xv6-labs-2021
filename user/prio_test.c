#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Simple test program to demonstrate priority scheduling in xv6.
// It creates several child processes with different priorities and
// lets them run CPU-bound loops. Higher-priority processes (smaller
// priority value) should tend to finish earlier.

int
main(int argc, char *argv[])
{
  int n = 3;  // number of children
  int base_prio[3] = {0, 10, 20}; // 0 is highest, 20 is lowest

  printf("prio_test: creating %d CPU-bound processes with different priorities\n", n);

  for(int i = 0; i < n; i++){
    int pid = fork();
    if(pid < 0){
      printf("prio_test: fork failed\n");
      exit(1);
    }
    if(pid == 0){
      // child
      int prio = base_prio[i];
      if(setpriority(prio) < 0){
        printf("child %d: setpriority(%d) failed\n", getpid(), prio);
        exit(1);
      }

      int start = uptime();
      // CPU-bound loop: do some busy work
      volatile int sink = 0;
      for(int k = 0; k < 50; k++){
        for(volatile int j = 0; j < 1000000; j++){
          sink += j;
        }
      }
      int end = uptime();

      printf("child pid=%d prio=%d ran from tick %d to %d (delta=%d)\n",
             getpid(), prio, start, end, end - start);

      exit(0);
    }
    // parent continues loop and creates next child
  }

  // parent waits for all children
  for(int i = 0; i < n; i++){
    wait(0);
  }

  printf("prio_test: done\n");
  exit(0);
}


