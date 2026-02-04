#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#define POOL_SIZE 4    // 미리 생성할 쓰레드 개수
#define QUEUE_SIZE 10  // 작업 큐의 최대 크기

// 1. 개별 작업(Job)을 정의하는 구조체
typedef struct {
    void (*function)(void* arg); // 실행할 함수 포인터
    void* arg;                   // 함수에 넘길 인자
} threadpool_job_t;

// 2. 쓰레드 풀 제어 구조체
typedef struct {
    pthread_mutex_t lock;        // 큐 접근 보호용 뮤텍스
    pthread_cond_t notify;       // 새 작업 알림용 조건 변수
    pthread_t threads[POOL_SIZE]; // 워커 쓰레드 식별자 배열
    threadpool_job_t queue[QUEUE_SIZE]; // 작업 대기 큐 (환형 배열)
    int head, tail, count;       // 큐 관리를 위한 인덱스 및 개수
    int shutdown;                // 풀 종료 플래그
} threadpool_t;

threadpool_t *pool;

//큐 관련 함수
void queue_init(threadpool_t *pool);
void queue_destroy(threadpool_t *pool);
void queue_push(threadpool_t *pool, threadpool_job_t job);
threadpool_job_t queue_pop(threadpool_t *pool);
int queue_size(threadpool_t *pool);
int queue_empty(threadpool_t *pool);
int queue_full(threadpool_t *pool);
threadpool_job_t* queue_front(threadpool_t *pool);
threadpool_job_t* queue_back(threadpool_t *pool);

//스레드 관련 함수
void thread_pool_init(threadpool_t *pool);
void thread_pool_destroy(threadpool_t *pool);
void thread_pool_add_job(threadpool_t *pool, threadpool_job_t job);
void thread_pool_wait(threadpool_t *pool);
void thread_pool_shutdown(threadpool_t *pool);

#ifdef __cplusplus
}
#endif

#endif // THREAD_POOL_H
