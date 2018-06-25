#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

struct param
{
    int start;
    int end;
};

struct result
{
    double sum;
};

void *thread(void *argc)
{
    struct param *param;
    struct result *result;
    double sum = 0, number = 0;
    int i;
    int flag = 0;

    param = (struct param *)argc;
    for (i = param->start, flag = (i / 2) % 2; i <= param->end; i = i + 2)
    {
        flag = flag ^ 1;
        number = 1.0 / i;
        sum += flag ? number : -number;
    }
    result = malloc(sizeof(struct result));
    result->sum = sum;
    return result;
}

#define TOTAL 1000
#define CPU 5
#define CHILD (TOTAL / CPU)

int main()
{
    pthread_t workers[CPU];
    struct param params[CPU];
    int i;

    for (i = 0; i < CPU; i++)
    {
        struct param *param;
        param = &params[i];
        param->start = i * CHILD + 1;
        param->end = (i + 1) * CHILD;
        pthread_create(&workers[i], NULL, thread, param);
    }

    double sum = 0;
    for (i = 0; i < CPU; i++)
    {
        struct result *result;
        pthread_join(workers[i], &result);
        sum += result->sum;
        free(result);
    }

    printf("value of pi = %.8lf\n", sum * 4);
    return 0;
}