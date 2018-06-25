/**
ring.c: 创建N个线程，它们构成一个环

创建N个线程：T1、T2、T3、… TN
T1向T2发送整数1
T2收到后将整数加1
T2向T3发送整数2
T3收到后将整数加1
T3向T4发送整数3
…
TN收到后将整数加1
TN向T1发送整数N
*/

#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>

struct index{
	int i;
};
int num;
#define COUNT 10


sem_t wait_num[COUNT];
sem_t mutex;
pthread_t ring_tid[COUNT];

void *send(void *arg) {
	
	struct index *index;
	index = (struct index *)arg;
	
	sem_wait(&wait_num[index->i]);
	sem_wait(&mutex);
	
	num++;
	
	printf("T%d send %d to T%d\n", index->i + 1, num, (index->i + 2) % COUNT);
	
	int next = (index->i + 1) % COUNT;
	sem_post(&mutex);
	sem_post(&wait_num[next]);
	
	
	if(index->i == 0)
	{
		int count_join = 1;
		while(count_join < COUNT){
			pthread_join(ring_tid[count_join], NULL);
			count_join++;
		}
	}
	
	return NULL;
}

int main() 
{
	int i;
	
	struct index indexs[COUNT];
	
    sem_init(&mutex, 0,1);
	for(i = 0; i < COUNT; i++)
	{
		struct index *index;
		index = &indexs[i];
		index->i = i;
		sem_init(&wait_num[i],0, 0);
		pthread_create(&ring_tid[i], NULL, send, index);
	}
	
	sem_post(&wait_num[0]);	
	pthread_join(ring_tid[0], NULL);
}