struct stat;
struct rtcdate;

// Custom system call structures
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

// system calls
int fork(void);
int exit(int) __attribute__((noreturn));
int wait(int*);
int pipe(int*);
int write(int, const void*, int);
int read(int, void*, int);
int close(int);
int kill(int);
int exec(char*, char**);
int open(const char*, int);
int mknod(const char*, short, short);
int unlink(const char*);
int fstat(int fd, struct stat*);
int link(const char*, const char*);
int mkdir(const char*);
int chdir(const char*);
int dup(int);
int getpid(void);
char* sbrk(int);
int sleep(int);
int uptime(void);
int getprocinfo(struct procinfo*);
int getsystime(struct systime*);
int setpriority(int pid, int priority);

// ulib.c
int stat(const char*, struct stat*);
char* strcpy(char*, const char*);
void *memmove(void*, const void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
void fprintf(int, const char*, ...);
void printf(const char*, ...);
char* gets(char*, int max);
uint strlen(const char*);
void* memset(void*, int, uint);
void* malloc(uint);
void free(void*);
int atoi(const char*);
int memcmp(const void *, const void *, uint);
void *memcpy(void *, const void *, uint);
