#include <iostream>
#include <pthread.h>
#include <thread>

#define NUM_THREAD 5

struct threadData{
    int thread_id;
    double message;
};

void *printHello(void *args){
    struct threadData * data = (struct threadData *)args;
    printf("thread id: %d\n", data->thread_id);
    printf("message: %f\n", data->message);
//    pthread_exit(NULL);
}

int main() {
    pthread_t thread[NUM_THREAD];
    struct threadData td[NUM_THREAD];

    for(int i=0; i<NUM_THREAD; ++i){
        td[i].thread_id = i;
        td[i].message = i;
        int ret = pthread_create(&(thread[i]), NULL, printHello, (void*)&(td[i]));
        if(ret != 0){
            printf("thread %d create error!, error code: %d\n", i, ret);
            continue;
        }
//        pthread_join(thread[i], NULL);
    }

    pthread_exit(NULL);
    return 0;
}
