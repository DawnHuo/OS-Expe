#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define MAX 1000000
double result[2];

void *thread(void *argc)
{
    int *arg = (int *)argc;
    double add = 0, number = 0;
    int flag = 0;
    int i;
    for (i = arg[0]; i < arg[i]; i++)
    {
        flag = flag ^ 1;
        number = 1.0 / (2 * i - 1);
        add += flag ? number : -number;
    }
    result[arg[2]] = add;
}

int main()
{
    pthread_t tid;
    int n = MAX, arra[3], arrb[3];
    arra[0] = 1;
    arra[1] = n / 2;
    arra[2] = 0;
    arrb[0] = n / 2 + 1;
    arrb[1] = n;
    arrb[2] = 1;

    pthread_create(&tid, NULL, &thread, (void *)arrb);
    thread((void *)arra);
    pthread_join(tid, NULL);
    printf("value of pi = %.8lf\n", 4 * (result[0] + result[1]));
    return 0;
}