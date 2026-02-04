#include "thread_pool.h"

// 워커 스레드 함수
static void* worker_thread(void* arg) {
    threadpool_t* pool = (threadpool_t*)arg;
    
    while(1) {
        threadpool_job_t job = queue_pop(pool);
        
        // shutdown이고 큐가 비어있으면 종료
        if(job.function == NULL && pool->shutdown) {
            break;
        }
        
        // 작업 실행
        if(job.function != NULL) {
            job.function(job.arg);
        }
    }
    
    return NULL;
}

void thread_pool_add_job(threadpool_t *pool, threadpool_job_t job)
{
    // queue_push 내부에서 뮤텍스 락을 처리하므로 여기서는 직접 호출만
    queue_push(pool, job);
}

void thread_pool_wait(threadpool_t *pool)
{
    pthread_mutex_lock(&pool->lock);
    while(queue_size(pool) > 0 && !pool->shutdown) {
        pthread_cond_wait(&pool->notify, &pool->lock);
    }
    pthread_mutex_unlock(&pool->lock);
}

void thread_pool_shutdown(threadpool_t *pool)
{
    if(pool == NULL) return;
    
    pthread_mutex_lock(&pool->lock);
    pool->shutdown = 1;
    pthread_mutex_unlock(&pool->lock);
    
    // 모든 대기 중인 스레드를 깨움
    pthread_cond_broadcast(&pool->notify);
    
    // 모든 워커 스레드 종료 대기
    for(int i = 0; i < POOL_SIZE; i++) {
        pthread_join(pool->threads[i], NULL);
    }
}

void thread_pool_init(threadpool_t *pool)
{
    if(pool == NULL) return;
    
    // 초기화
    pool->head = 0;
    pool->tail = 0;
    pool->count = 0;
    pool->shutdown = 0;
    
    pthread_mutex_init(&pool->lock, NULL);
    pthread_cond_init(&pool->notify, NULL);
    
    // 워커 스레드 생성
    for(int i = 0; i < POOL_SIZE; i++) {
        pthread_create(&pool->threads[i], NULL, worker_thread, (void*)pool);
    }
}
void thread_pool_destroy(threadpool_t *pool)
{
    if(pool == NULL) return;
    
    // shutdown 호출로 스레드 종료
    thread_pool_shutdown(pool);
    
    // 뮤텍스와 조건 변수 제거
    pthread_mutex_destroy(&pool->lock);
    pthread_cond_destroy(&pool->notify);
}



//큐 관련 함수
int queue_size(threadpool_t *pool)
{
    if(pool == NULL) return -1;

    return pool->count;
}
int queue_empty(threadpool_t *pool)
{
    if(pool == NULL) return -1;

    return pool->count == 0;
}
int queue_full(threadpool_t *pool)
{
    if(pool == NULL) return -1;

    return pool->count == QUEUE_SIZE;
}
threadpool_job_t* queue_front(threadpool_t *pool)
{
    if(pool == NULL) return NULL;
    if(queue_empty(pool)) return NULL;

    return &pool->queue[pool->head];
}
threadpool_job_t* queue_back(threadpool_t *pool)
{
    if(pool == NULL) return NULL;
    if(queue_empty(pool)) return NULL;

    int back_idx = (pool->tail - 1 + QUEUE_SIZE) % QUEUE_SIZE;
    return &pool->queue[back_idx];
}

void queue_destroy(threadpool_t *pool) {
    if(pool == NULL) return;
    // queue와 threads는 정적 배열이므로 free할 필요 없음
}

void queue_push(threadpool_t *pool, threadpool_job_t job) {
    if(pool == NULL) return;

    pthread_mutex_lock(&pool->lock);
    while(queue_full(pool) && !pool->shutdown) {
        pthread_cond_wait(&pool->notify, &pool->lock);
    }

    if(pool->shutdown) {
        pthread_mutex_unlock(&pool->lock);
        return;
    }

    pool->queue[pool->tail] = job;
    pool->tail = (pool->tail + 1) % QUEUE_SIZE;
    pool->count++;
    pthread_mutex_unlock(&pool->lock);
    pthread_cond_signal(&pool->notify);
}

threadpool_job_t queue_pop(threadpool_t *pool) {
    if(pool == NULL) {
        threadpool_job_t empty_job = {NULL, NULL};
        return empty_job;
    }

    pthread_mutex_lock(&pool->lock);
    while(queue_empty(pool) && !pool->shutdown) {
        pthread_cond_wait(&pool->notify, &pool->lock);
    }

    if(pool->shutdown && queue_empty(pool)) {
        pthread_mutex_unlock(&pool->lock);
        threadpool_job_t empty_job = {NULL, NULL};
        return empty_job;
    }

    threadpool_job_t job = pool->queue[pool->head];
    pool->head = (pool->head + 1) % QUEUE_SIZE;
    pool->count--;
    pthread_mutex_unlock(&pool->lock);
    pthread_cond_signal(&pool->notify);  // 큐가 비어갈 수 있으므로 thread_pool_wait를 깨움

    return job;
}