#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

typedef struct
{
    int value;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} sema_t;

void sema_init(sema_t *sema, int value)
{
    sema->value = value;
    pthread_mutex_init(&sema->mutex, NULL);
    pthread_cond_init(&sema->cond, NULL);
}

void sema_wait(sema_t *sema)
{
    pthread_mutex_lock(&sema->mutex);
    while (sema->value <= 0)
        pthread_cond_wait(&sema->cond, &sema->mutex);
    sema->value--;
    pthread_mutex_unlock(&sema->mutex);
}

void sema_signal(sema_t *sema)
{
    pthread_mutex_lock(&sema->mutex);
    ++sema->value;
    pthread_cond_signal(&sema->cond);
    pthread_mutex_unlock(&sema->mutex);
}

#define CAPACITY 4
int buffer1[CAPACITY], buffer2[CAPACITY];
int in1, in2;
int out1, out2;

int buffer1_is_empty()
{
    return in1 == out1;
}

int buffer1_is_full()
{
    return (in1 + 1) % CAPACITY == out1;
}

int buffer2_is_empty()
{
    return in2 == out2;
}

int buffer2_is_full()
{
    return (in2 + 1) % CAPACITY == out2;
}

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
sema_t mutex1_sema, mutex2_sema;
sema_t empty_buffer1_sema, empty_buffer2_sema;
sema_t full_buffer1_sema, full_buffer2_sema;

#define ITEM_COUNT (CAPACITY * 2)
void *consume(void *arg)
{
    int i;
    int item;

    for (i = 0; i < ITEM_COUNT; i++)
    {
        sema_wait(&full_buffer2_sema);
        sema_wait(&mutex2_sema);

        item = get_item2();

        printf("		consume item: %c\n", item);

        sema_signal(&mutex2_sema);
        sema_signal(&empty_buffer2_sema);
    }
    return NULL;
}

void *compute(void *arg)
{
    int i;
    int item;

    for (i = 0; i < ITEM_COUNT; i++)
    {
        sema_wait(&full_buffer1_sema);
        sema_wait(&mutex1_sema);

        item = get_item1();
        item = item - 32;
        printf("	transfer item: %c\n", item);
        sema_signal(&mutex1_sema);
        sema_signal(&empty_buffer1_sema);

        sema_wait(&empty_buffer2_sema);
        sema_wait(&mutex2_sema);

        put_item2(item);

        sema_signal(&mutex2_sema);
        sema_signal(&full_buffer2_sema);
    }
}

void produce()
{
    int i;
    int item;

    for (i = 0; i < ITEM_COUNT; i++)
    {
        sema_wait(&empty_buffer1_sema);
        sema_wait(&mutex1_sema);

        item = i + 'a';
        put_item1(item);
        printf("produce item: %c\n", item);

        sema_signal(&mutex1_sema);
        sema_signal(&full_buffer1_sema);
    }
}

// The program contains a bug. Can you find it?
int main()
{
    pthread_t consumer_tid, computer_tid;

    sema_init(&mutex1_sema, 1);
    sema_init(&mutex2_sema, 1);
    sema_init(&empty_buffer1_sema, CAPACITY - 1);
    sema_init(&empty_buffer2_sema, CAPACITY - 1);
    sema_init(&full_buffer1_sema, 0);
    sema_init(&full_buffer2_sema, 0);

    pthread_create(&computer_tid, NULL, compute, NULL);
    pthread_create(&consumer_tid, NULL, consume, NULL);
    produce();
    pthread_join(consumer_tid, NULL);
    return 0;
}