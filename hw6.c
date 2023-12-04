#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_THREADS 16
#define BUFFER_SIZE 10 //max buffer size is 10

/*
12/4 Zahradinee
- created the producer and consumer functions according to the hw specifications
- currently the output does not match up with the sample outputs so have to debug the functions
*/

// global
int buffer[BUFFER_SIZE];
int itemNum = 0;
int writeIndex = 0;
int readIndex = 0;
int itemsProduced = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t buffer_not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffer_not_empty = PTHREAD_COND_INITIALIZER;

int p, c, i, b, d;

// Producer function
void *producer(void *args) {
    int producer_id = *((int *)args);
    int items_to_produce = i;

    for (int j = 0; j < items_to_produce; ++j) {
        int item = producer_id * items_to_produce + j;

        pthread_mutex_lock(&mutex);
        while (itemNum == BUFFER_SIZE) {
            pthread_cond_wait(&buffer_not_full, &mutex);
        }

        buffer[writeIndex] = item;
        writeIndex = (writeIndex + 1) % BUFFER_SIZE;
        itemNum++;

        printf("producer_%d produced item %d\n", producer_id, item);

        pthread_cond_signal(&buffer_not_empty);
        pthread_mutex_unlock(&mutex);

        if (d == 1) {
            usleep(500000); // 0.5s delay
        }
    }

    pthread_exit(NULL);
}

// consumer function
void *consumer(void *args) {
    int consumer_id = *((int *)args);
    int total_items = p * i;
    int items_to_consume_per_consumer = total_items / c;

    for (int j = 0; j < items_to_consume_per_consumer; ++j) {
        pthread_mutex_lock(&mutex);
        while (itemNum == 0) {
            pthread_cond_wait(&buffer_not_empty, &mutex);
        }

        int item = buffer[readIndex];
        readIndex = (readIndex + 1) % BUFFER_SIZE;
        itemNum--;

        printf("consumer_%d consumed item %d\n", consumer_id, item);

        pthread_cond_signal(&buffer_not_full);
        pthread_mutex_unlock(&mutex);

        if (d == 0) {
            usleep(500000); // 0.5s delay
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 6) {
        printf("Usage: ./a.out p c i b d\n");
        return 1;
    }

    p = atoi(argv[1]);
    c = atoi(argv[2]);
    i = atoi(argv[3]);
    b = atoi(argv[4]);
    d = atoi(argv[5]);

    pthread_t producer_threads[MAX_THREADS];
    pthread_t consumer_threads[MAX_THREADS];
    int producer_args[MAX_THREADS];
    int consumer_args[MAX_THREADS];

    if (c >= p * i) {
        printf("Error: Number of consumers should be smaller than the total items being produced.\n");
        return 1;
    }

    //producer
    for (int j = 0; j < p; ++j) {
        producer_args[j] = j;
        pthread_create(&producer_threads[j], NULL, producer, &producer_args[j]);
    }

    //consumer
    for (int j = 0; j < c; ++j) {
        consumer_args[j] = j;
        pthread_create(&consumer_threads[j], NULL, consumer, &consumer_args[j]);
    }

    for (int j = 0; j < p; ++j) {
        pthread_join(producer_threads[j], NULL);
    }

    for (int j = 0; j < c; ++j) {
        pthread_join(consumer_threads[j], NULL);
    }

    return 0;
}
