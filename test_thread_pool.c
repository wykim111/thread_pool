#include "thread_pool.h"
#include <stdio.h>
#include <unistd.h>
#include <time.h>

// 작업 데이터 구조체
typedef struct {
    int id;
    int duration;  // 작업 소요 시간 (초)
} task_data_t;

// 테스트 작업 함수 1: 간단한 출력
void simple_task(void* arg) {
    task_data_t* data = (task_data_t*)arg;
    printf("Task %d: Starting (duration: %d seconds)\n", data->id, data->duration);
    sleep(data->duration);
    printf("Task %d: Completed\n", data->id);
    free(data);
}

// 테스트 작업 함수 2: 계산 작업
void compute_task(void* arg) {
    int* num = (int*)arg;
    int result = 0;
    for(int i = 0; i < *num * 1000000; i++) {
        result += i;
    }
    printf("Compute task: Calculated sum for %d (result: %d)\n", *num, result);
    free(num);
}

// 테스트 작업 함수 3: 간단한 메시지 출력
void message_task(void* arg) {
    char* msg = (char*)arg;
    printf("Message: %s\n", msg);
    free(msg);
}

int main() {
    printf("=== Thread Pool Test ===\n\n");
    
    // 스레드 풀 구조체 선언
    threadpool_t pool;
    
    // 스레드 풀 초기화
    printf("1. Initializing thread pool...\n");
    thread_pool_init(&pool);
    printf("   Thread pool initialized with %d worker threads\n\n", POOL_SIZE);
    
    // 작업 추가
    printf("2. Adding jobs to the pool...\n");
    
    // 작업 1-5: 시간이 걸리는 작업들
    for(int i = 1; i <= 5; i++) {
        task_data_t* data = (task_data_t*)malloc(sizeof(task_data_t));
        data->id = i;
        data->duration = i % 3 + 1;  // 1, 2, 또는 3초
        
        threadpool_job_t job;
        job.function = simple_task;
        job.arg = data;
        
        thread_pool_add_job(&pool, job);
        printf("   Added task %d (duration: %d seconds)\n", i, data->duration);
    }
    
    // 작업 6-8: 계산 작업들
    for(int i = 6; i <= 8; i++) {
        int* num = (int*)malloc(sizeof(int));
        *num = i;
        
        threadpool_job_t job;
        job.function = compute_task;
        job.arg = num;
        
        thread_pool_add_job(&pool, job);
        printf("   Added compute task %d\n", i);
    }
    
    // 작업 9-10: 메시지 작업들
    char* msg1 = (char*)malloc(50);
    strcpy(msg1, "Hello from thread pool!");
    threadpool_job_t job1;
    job1.function = message_task;
    job1.arg = msg1;
    thread_pool_add_job(&pool, job1);
    printf("   Added message task 1\n");
    
    char* msg2 = (char*)malloc(50);
    strcpy(msg2, "Thread pool is working!");
    threadpool_job_t job2;
    job2.function = message_task;
    job2.arg = msg2;
    thread_pool_add_job(&pool, job2);
    printf("   Added message task 2\n");
    
    printf("\n3. All jobs added. Waiting for completion...\n");
    printf("   (Worker threads will process jobs concurrently)\n\n");
    
    // 모든 작업 완료 대기
    thread_pool_wait(&pool);
    
    printf("\n4. All jobs completed!\n");
    
    // 스레드 풀 종료
    printf("5. Destroying thread pool...\n");
    thread_pool_destroy(&pool);
    printf("   Thread pool destroyed\n");
    
    printf("\n=== Test Completed Successfully ===\n");
    
    return 0;
}

