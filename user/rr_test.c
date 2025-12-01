#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// rr_test: demonstrate approximate round-robin scheduling in xv6.
// It creates several identical CPU-bound child processes.
// Each child periodically prints its pid and a step counter.
// From the interleaving of output you can observe that
// processes are being preempted and scheduled in a round-robin fashion.

static void
worker(int id)
{
  volatile int sink = 0;
  for(int step = 0; step < 50; step++){
    // busy work for a while
    for(volatile int i = 0; i < 1000000; i++){
      sink += i;
    }
    // periodically print a line so we can see scheduling order
    printf("worker %d (pid=%d) step %d\n", id, getpid(), step);
  }
}

int
main(int argc, char *argv[])
{
  int n = 3; // default number of workers

  if(argc > 1){
    n = atoi(argv[1]);
    if(n <= 0)
      n = 3;
  }

  printf("rr_test: creating %d CPU-bound workers\n", n);

  for(int i = 0; i < n; i++){
    int pid = fork();
    if(pid < 0){
      printf("rr_test: fork failed\n");
      exit(1);
    }
    if(pid == 0){
      // child
      worker(i);
      exit(0);
    }
  }

  // parent waits for all children
  for(int i = 0; i < n; i++){
    wait(0);
  }

  printf("rr_test: done\n");
  exit(0);
}


