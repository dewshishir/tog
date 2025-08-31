#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define SIZE 5  // Buffer size

int *buffer;
int in = 0, out = 0;
int total_items;

sem_t empty;
sem_t full;
pthread_mutex_t mutex;

void* producer(void* arg) {
    int id = *(int*)arg; // Dereference the pointer to get the ID
    for (int i = 0; i < total_items; i++) {
        int item = rand() % 100;
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        buffer[in] = item;
        printf("[Producer %d] Produced: %d\n", id, item);
        in = (in + 1) % SIZE;
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
        sleep(rand() % 2);
    }
    pthread_exit(NULL);
}

void* consumer(void* arg) {
    int id = *(int*)arg; // Dereference the pointer to get the ID
    for (int i = 0; i < total_items; i++) {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        int item = buffer[out];
        printf("[Consumer %d] Consumed: %d\n", id, item);
        out = (out + 1) % SIZE;
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
        sleep(rand() % 2);
    }
    pthread_exit(NULL);
}

int main() {
    int num_producers, num_consumers;

    printf("Enter number of producers: ");
    scanf("%d", &num_producers);
    printf("Enter number of consumers: ");
    scanf("%d", &num_consumers);
    printf("Enter number of items per producer/consumer: ");
    scanf("%d", &total_items);

    buffer = (int*)malloc(SIZE * sizeof(int));

    sem_init(&empty, 0, SIZE);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    pthread_t prod_threads[num_producers], cons_threads[num_consumers];
    int ids[num_producers > num_consumers ? num_producers : num_consumers];

    for (int i = 0; i < num_producers || i < num_consumers; i++) {
        ids[i] = i + 1;
        if (i < num_producers)
            pthread_create(&prod_threads[i], NULL, producer, &ids[i]);
        if (i < num_consumers)
            pthread_create(&cons_threads[i], NULL, consumer, &ids[i]);
    }

    for (int i = 0; i < num_producers; i++)
        pthread_join(prod_threads[i], NULL);
    for (int i = 0; i < num_consumers; i++)
        pthread_join(cons_threads[i], NULL);

    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);
    free(buffer);

    return 0;
}
