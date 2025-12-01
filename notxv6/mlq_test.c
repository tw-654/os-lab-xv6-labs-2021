#include <stdio.h>

// Multi-level feedback queue scheduling test (user-space simulator).
// 用于在实验报告中展示多级队列调度在不同负载下的性能表现。

typedef enum {
  READY,
  RUNNING,
  FINISHED
} State;

typedef struct {
  int pid;
  int arrival_time;   // 到达时间
  int burst_time;     // 总 CPU 需求时间

  int queue_level;    // 所属队列 0=高,1=中,2=低

  // 运行时字段
  int remaining_time;
  int start_time;     // 第一次开始运行时间
  int finish_time;    // 完成时间
  int first_response; // 第一次被调度时间
  State state;
} Process;

typedef struct {
  int q[64];
  int front, rear, size;
} Queue;

static void
q_init(Queue *q)
{
  q->front = q->rear = q->size = 0;
}

static int
q_empty(Queue *q)
{
  return q->size == 0;
}

static void
q_push(Queue *q, int x)
{
  if(q->size >= 64) return;
  q->q[q->rear] = x;
  q->rear = (q->rear + 1) % 64;
  q->size++;
}

static int
q_pop(Queue *q)
{
  if(q->size == 0) return -1;
  int x = q->q[q->front];
  q->front = (q->front + 1) % 64;
  q->size--;
  return x;
}

static void
init_runtime(Process ps[], int n)
{
  for(int i = 0; i < n; i++){
    ps[i].remaining_time = ps[i].burst_time;
    ps[i].start_time = -1;
    ps[i].finish_time = -1;
    ps[i].first_response = -1;
    ps[i].state = READY;
  }
}

static float
avg_wait(Process ps[], int n)
{
  int total = 0;
  for(int i = 0; i < n; i++){
    int turnaround = ps[i].finish_time - ps[i].arrival_time;
    int wait = turnaround - ps[i].burst_time;
    total += wait;
  }
  return (float)total / n;
}

static float
avg_turn(Process ps[], int n)
{
  int total = 0;
  for(int i = 0; i < n; i++){
    total += ps[i].finish_time - ps[i].arrival_time;
  }
  return (float)total / n;
}

static float
avg_resp(Process ps[], int n)
{
  int total = 0;
  for(int i = 0; i < n; i++){
    total += ps[i].first_response - ps[i].arrival_time;
  }
  return (float)total / n;
}

// 多级队列调度：三个队列，高->中->低 优先，队列内为 RR。
void
mlq_schedule(Process ps[], int n, int q_high, int q_med, int q_low)
{
  Queue high, med, low;
  q_init(&high);
  q_init(&med);
  q_init(&low);

  int time = 0;
  int finished = 0;
  int enq[64] = {0};

  while(finished < n){
    // 把在当前时刻到达的进程加入对应队列
    for(int i = 0; i < n; i++){
      if(ps[i].arrival_time == time && !enq[i]){
        if(ps[i].queue_level == 0)      q_push(&high, i);
        else if(ps[i].queue_level == 1) q_push(&med, i);
        else                            q_push(&low, i);
        enq[i] = 1;
      }
    }

    Queue *curq = 0;
    int qlen = 0;

    if(!q_empty(&high)){
      curq = &high;
      qlen = q_high;
    } else if(!q_empty(&med)){
      curq = &med;
      qlen = q_med;
    } else if(!q_empty(&low)){
      curq = &low;
      qlen = q_low;
    } else {
      // 没有就绪进程，时间前进
      time++;
      continue;
    }

    int idx = q_pop(curq);
    Process *p = &ps[idx];

    if(p->first_response < 0)
      p->first_response = time;
    if(p->start_time < 0)
      p->start_time = time;

    int slice = qlen;
    while(slice > 0 && p->remaining_time > 0){
      p->remaining_time--;
      time++;
      slice--;

      // 每个时间单位检查新到达进程
      for(int i = 0; i < n; i++){
        if(ps[i].arrival_time == time && !enq[i]){
          if(ps[i].queue_level == 0)      q_push(&high, i);
          else if(ps[i].queue_level == 1) q_push(&med, i);
          else                            q_push(&low, i);
          enq[i] = 1;
        }
      }
    }

    if(p->remaining_time == 0){
      p->finish_time = time;
      p->state = FINISHED;
      finished++;
    } else {
      // 简单版本：不做队列间迁移，留在原队列
      if(p->queue_level == 0)      q_push(&high, idx);
      else if(p->queue_level == 1) q_push(&med, idx);
      else                         q_push(&low, idx);
    }
  }
}

static void
run_and_print(const char *title, Process ps[], int n,
              int qh, int qm, int ql)
{
  init_runtime(ps, n);
  mlq_schedule(ps, n, qh, qm, ql);

  printf("=== %s ===\n", title);
  printf("PID\tArr\tBurst\tQ\tStart\tFinish\tWait\tTurn\tResp\n");
  for(int i = 0; i < n; i++){
    int turn = ps[i].finish_time - ps[i].arrival_time;
    int wait = turn - ps[i].burst_time;
    int resp = ps[i].first_response - ps[i].arrival_time;
    printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
           ps[i].pid,
           ps[i].arrival_time,
           ps[i].burst_time,
           ps[i].queue_level,
           ps[i].start_time,
           ps[i].finish_time,
           wait,
           turn,
           resp);
  }
  printf("Avg waiting time:    %.2f\n", avg_wait(ps, n));
  printf("Avg turnaround time: %.2f\n", avg_turn(ps, n));
  printf("Avg response time:   %.2f\n\n", avg_resp(ps, n));
}

int
main(void)
{
  // 1) 纯 CPU 密集型 + 不同队列级别
  Process cpu_mix[] = {
    // pid, arr, burst, queue
    {1, 0, 30, 0},  // 高优队列
    {2, 0, 30, 1},  // 中队列
    {3, 0, 30, 2},  // 低队列
  };
  int n1 = sizeof(cpu_mix) / sizeof(cpu_mix[0]);
  run_and_print("CPU mix (MLQ: q_high=2,q_med=4,q_low=8)", cpu_mix, n1, 2, 4, 8);

  // 2) 不同到达时间 + 多级队列
  Process staggered[] = {
    {1, 0, 10, 0},  // 交互型，高队列
    {2, 2, 20, 1},  // 中等
    {3, 4, 30, 2},  // 批处理
    {4, 6, 5,  0},  // 交互型
  };
  int n2 = sizeof(staggered) / sizeof(staggered[0]);
  run_and_print("Staggered arrival (MLQ: 2/4/8)", staggered, n2, 2, 4, 8);

  // 3) 混合工作负载：多交互 + 多批处理
  Process mixed[] = {
    // 交互型（短作业，高队列）
    {1,  0, 3, 0},
    {2,  1, 4, 0},
    {3,  2, 2, 0},
    // 批处理型（长作业，低队列）
    {4,  0, 25, 2},
    {5,  3, 30, 2},
    {6,  5, 40, 2},
  };
  int n3 = sizeof(mixed) / sizeof(mixed[0]);
  run_and_print("Mixed workload (MLQ: 2/4/8)", mixed, n3, 2, 4, 8);

  return 0;
}


