#include "kernel/types.h"
#include "user/user.h"

void test_getprocinfo() {
    printf("Testing getprocinfo()...\n");
    
    struct procinfo info;
    if(getprocinfo(&info) == 0) {
        printf("  PID: %d\n", info.pid);
        printf("  PPID: %d\n", info.ppid);
        printf("  State: %d\n", info.state);
        printf("  Size: %d bytes\n", info.sz);
        printf("  Name: %s\n", info.name);
        printf("getprocinfo test: PASSED\n");
    } else {
        printf("getprocinfo test: FAILED\n");
    }
    printf("\n");
}

void test_getsystime() {
    printf("Testing getsystime()...\n");
    
    struct systime time;
    if(getsystime(&time) == 0) {
        printf("  Ticks: %d\n", time.ticks);
        printf("  Uptime: %d seconds\n", time.uptime);
        printf("getsystime test: PASSED\n");
    } else {
        printf("getsystime test: FAILED\n");
    }
    printf("\n");
}

void test_setpriority() {
    printf("Testing setpriority()...\n");
    
    int pid = getpid();
    
    // Test normal case
    if(setpriority(pid, 5) == 0) {
        printf("Set priority to 5: PASSED\n");
    } else {
        printf("Set priority to 5: FAILED\n");
    }
    
    // Test boundary condition - invalid priority (negative)
    if(setpriority(pid, -1) == -1) {
        printf("Invalid priority test (negative): PASSED\n");
    } else {
        printf("Invalid priority test (negative): FAILED\n");
    }
    
    // Test boundary condition - invalid priority (too high)
    if(setpriority(pid, 11) == -1) {
        printf("Invalid priority test (too high): PASSED\n");
    } else {
        printf("Invalid priority test (too high): FAILED\n");
    }
    
    // Test invalid PID
    if(setpriority(99999, 5) == -1) {
        printf("Invalid PID test: PASSED\n");
    } else {
        printf("Invalid PID test: FAILED\n");
    }
    
    printf("\n");
}

int main() {
    printf("Starting system call tests...\n");
    printf("================================\n\n");
    
    test_getprocinfo();
    test_getsystime();
    test_setpriority();
    
    printf("All tests completed!\n");
    exit(0);
}

