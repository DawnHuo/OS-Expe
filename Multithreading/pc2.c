#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define CAPACITY 4
int buffer1[CAPACITY];
int buffer2[CAPACITY];

int in1, in2;
int out1, out2;

int get_item1()
{
    int item;

    item = buffer1[out1];
    out1 = (out1 + 1) % CAPACITY;
    return item;
}

void put_item1(int item)
{
    buffer1[in1] = item;
    in1 = (in1 + 1) % CAPACITY;
}

int get_item2()
{
    int item;

    item = buffer2[out2];
    out2 = (out2 + 1) % CAPACITY;
    return item;
}

void put_item2(int item)
{
    buffer2[in2] = item;
    in2 = (in2 + 1) % CAPACITY;
}

sem_t mutex1;
sem_t mutex2;
sem_t empty_buffer1;
sem_t full_buffer1;
sem_t empty_buffer2;
sem_t full_buffer2;

#define ITEM_COUNT (CAPACITY * 2)
void *consume(void *arg)
{
    int i;
    int item;

    for (i = 0; i < ITEM_COUNT; i++) {
        sem_wait(&full_buffer2);
        sem_wait(&mutex2);

        item = get_item2(); 
        printf("		consume item: %c\n", item);

        sem_post(&mutex2);
        sem_post(&empty_buffer2);
    }
    return NULL;
}

void *compute(void *arg)
{
	int i;
    int item;

    for (i = 0; i < ITEM_COUNT; i++) {
        sem_wait(&full_buffer1);
        sem_wait(&mutex1);
        
        item = get_item1();
        
        sem_post(&mutex1);
        sem_post(&empty_buffer1);
        
        
        sem_wait(&empty_buffer2);
        sem_wait(&mutex2);
        item = item - 32;
        put_item2(item);
        printf("	transferde : %c\n", item);

        sem_post(&mutex2);
        sem_post(&full_buffer2);
    }
    return NULL;
}

void produce()
{
    int i;
    int item;

    for (i = 0; i < ITEM_COUNT; i++) {
    	sem_wait(&empty_buffer1);
    	sem_wait(&mutex1);
    	
        item = i + 'a';
        put_item1(item);
        printf("produce item: %c\n", item);

		sem_post(&mutex1);
		sem_post(&full_buffer1);
    }
    
    
}

int main()
{
    pthread_t consumer_tid;
    pthread_t computer_tid;

    sem_init(&mutex1, 0, 1);
    sem_init(&mutex2, 0, 1);
    sem_init(&empty_buffer1, 0, CAPACITY-1);
    sem_init(&full_buffer1, 0, 0);
    sem_init(&empty_buffer2, 0, CAPACITY - 1);
    sem_init(&full_buffer2, 0, 0);


    pthread_create(&computer_tid, NULL, compute, NULL);
    pthread_create(&consumer_tid, NULL, consume, NULL);
    produce();
    
    pthread_join(consumer_tid, NULL);
    pthread_join(computer_tid, NULL);
    return 0;
}