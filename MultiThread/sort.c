#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct
{
    int *r;
    int length;
} SqList;

void SelectSort(SqList L, int start, int end)
{
    int tmp;
    int i, j;
    for (i = start; i < end; i++)
    {
        L.r[0] = i;
        for (j = i; j <= end; j++)
            if (L.r[j] < L.r[L.r[0]])
                L.r[0] = j;
        if (L.r[0] != i)
        {
            tmp = L.r[i];
            L.r[i] = L.r[L.r[0]];
            L.r[L.r[0]] = tmp;
        }
    }
}

void Merge(int SR[], int TR[], int i, int m, int n)
{
    int j, k;
    for (j = m + 1, k = i; j <= n && i <= m; ++k)
    {
        if (SR[i] < SR[j])
            TR[k] = SR[i++];
        else
            TR[k] = SR[j++];
    }
    if (i <= m)
        for (; i <= m; ++i, ++k)
            TR[k] = SR[i];
    if (j <= n)
        for (; j <= n; ++j, ++k)
            TR[k] = SR[j];
}

void MSort(int SR[], int TR1[], int s, int t)
{
    int m;
    int TR2[t + 1];
    if (s == t)
        TR1[s] = SR[s];
    else
    {
        m = (s + t) / 2;
        MSort(SR, TR2, s, m);
        MSort(SR, TR2, m + 1, t);
        Merge(TR2, TR1, s, m, t);
    }
}

void MergeSort(SqList L)
{
    MSort(L.r, L.r, 1, L.length);
}

void *thread(void *arg)
{
    SqList *L;
    L = (SqList *)arg;
    int s = L->length / 2 + 1;
    SelectSort(*L, s, L->length);
}

int main()
{
    SqList L;
    int i;
    int a[] = {0, 49, 38, 10, 76, 12, 43, 65};
    L.r = a;
    L.length = 7;

    pthread_t tid;

    pthread_create(&tid, NULL, thread, &L);
    pthread_join(tid, NULL);

    SelectSort(L, 1, L.length / 2);

    MergeSort(L);

    for (i = 0; i <= L.length; i++)
        printf("%d ", L.r[i]);
    printf("\n");
    return 0;
}