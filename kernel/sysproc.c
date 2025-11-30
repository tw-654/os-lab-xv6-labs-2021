#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

extern struct proc proc[NPROC];

// Structures for custom system calls (must match user/user.h)
struct procinfo {
  int pid;        // 进程ID
  int ppid;       // 父进程ID
  int state;      // 进程状态
  uint sz;        // 内存大小
  char name[16];  // 进程名称
};

struct systime {
  uint ticks;     // 系统时钟滴答数
  uint uptime;    // 系统运行时间（秒）
};

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// getprocinfo: return current process information
uint64
sys_getprocinfo(void)
{
  uint64 info_addr;
  struct proc *p = myproc();
  struct procinfo info;

  if(argaddr(0, &info_addr) < 0)
    return -1;

  // Fill in process information
  info.pid = p->pid;
  info.ppid = p->parent ? p->parent->pid : 0;
  info.state = p->state;
  info.sz = p->sz;
  safestrcpy(info.name, p->name, sizeof(info.name));

  // Copy to user space
  if(copyout(p->pagetable, info_addr, (char *)&info, sizeof(info)) < 0)
    return -1;

  return 0;
}

// getsystime: return system time information
uint64
sys_getsystime(void)
{
  uint64 time_addr;
  struct proc *p = myproc();
  struct systime time;

  if(argaddr(0, &time_addr) < 0)
    return -1;

  acquire(&tickslock);
  time.ticks = ticks;
  time.uptime = ticks / 100;  // Assuming 100 ticks = 1 second
  release(&tickslock);

  // Copy to user space
  if(copyout(p->pagetable, time_addr, (char *)&time, sizeof(time)) < 0)
    return -1;

  return 0;
}

// setpriority: set process priority
uint64
sys_setpriority(void)
{
  int pid, priority;
  struct proc *p;

  if(argint(0, &pid) < 0)
    return -1;
  if(argint(1, &priority) < 0)
    return -1;

  // Validate priority range (e.g., 0-10)
  if(priority < 0 || priority > 10)
    return -1;

  // Find the process
  for(p = proc; p < &proc[NPROC]; p++){
    acquire(&p->lock);
    if(p->pid == pid){
      p->priority = priority;
      release(&p->lock);
      return 0;
    }
    release(&p->lock);
  }

  return -1;  // Process not found
}