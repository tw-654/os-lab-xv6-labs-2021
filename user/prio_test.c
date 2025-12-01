#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Simple test program to demonstrate priority scheduling in xv6.
// It creates several child processes with different priorities and
// lets them run CPU-bound loops. Child processes only write their
// results to a pipe; the parent collects and prints results to avoid
// interleaved console output.

struct result {
  int pid;
  int prio;
  int delta;
};

int
main(int argc, char *argv[])
{
  int n = 3;  // number of children
  int base_prio[3] = {0, 10, 20}; // 0 is highest, 20 is lowest
  int fds[2];

  if(pipe(fds) < 0){
    printf("prio_test: pipe failed\n");
    exit(1);
  }

  printf("prio_test: creating %d CPU-bound processes with different priorities\n", n);

  for(int i = 0; i < n; i++){
    int pid = fork();
    if(pid < 0){
      printf("prio_test: fork failed\n");
      exit(1);
    }
    if(pid == 0){
      // child
      // close read end in child
      close(fds[0]);

      // child
      int prio = base_prio[i];
      if(setpriority(prio) < 0){
        printf("child %d: setpriority(%d) failed\n", getpid(), prio);
        exit(1);
      }

      int start = uptime();
      volatile int sink = 0;
      // 增大循环规模，让不同优先级在 tick 统计上更容易拉开差距
      for(int k = 0; k < 200; k++){
        for(volatile int j = 0; j < 2000000; j++){
          sink += j;
        }
      }
      int end = uptime();

      struct result r;
      r.pid = getpid();
      r.prio = prio;
      r.delta = end - start;

      // write result to parent
      write(fds[1], &r, sizeof(r));

      exit(0);
    }
    // parent continues loop and creates next child
  }

  // parent: close write end, only read
  close(fds[1]);

  // collect and print results from all children
  struct result r;
  for(int i = 0; i < n; i++){
    if(read(fds[0], &r, sizeof(r)) == sizeof(r)){
      printf("child pid=%d prio=%d ran (delta ticks=%d)\n",
             r.pid, r.prio, r.delta);
    } else {
      printf("prio_test: failed to read result for child %d\n", i);
    }
  }
  close(fds[0]);

  // parent waits for all children
  for(int i = 0; i < n; i++){
    wait(0);
  }

  printf("prio_test: done\n");
  exit(0);
}


