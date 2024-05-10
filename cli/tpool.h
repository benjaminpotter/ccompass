/// Thread pooling implementation.

#ifndef TPOOL_H
#define TPOOL_H

#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>


struct tpool_work {
    thread_func_t func;
    void *arg;
    struct tpool_work *next;
};
typedef struct tpool_work tpool_work_t;


struct tpool {
    tpool_work_t *work_first;
    tpool_work_t *work_last;
    pthread_mutex_t work_mutex;
    pthread_cond_t work_cond;
    pthread_cond_t working_cond;
    size_t working_cnt;
    size_t thread_cnt;
    bool stop;
};
typedef struct tpool tpool_t;


/// @brief Function pointer definition for thread work callback.
///
/// @param arg Data passed to the work callback.
typedef void (*thread_func_t)(void *arg);


/// @brief Creates a thread pool.
///
/// @param threads The number of threads to instantiate.
/// @returns A handle for the thread pool.
tpool_t *tpool_create(size_t threads);


/// @brief Destroys a thread pool.
///
/// @param tp The handle of the thread pool to destroy.
void tpool_destroy(tpool_t *tp);


/// @brief Adds work to the thread pool.
///
/// @param tp The handle of the thread pool to destroy.
/// @param func The work callback that gets called by the active thread.
/// @param arg Data passed to the work callback.
/// @returns True if the work was successfull added to the queue.
bool tpool_add_work(tpool_t *tp, thread_func_t func, void *arg);


/// @brief Blocks until all work in the queue is complete.
///
/// @param tp The handle of the thread pool to destroy.
void tpool_wait(tpool_t *tp);

#endif // TPOOL_H

