#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 * Implementation of 2-way partition quicksort.
 * Reference: http://www.sorting-algorithms.com/quick-sort
 */
void print_array(int * arr, int len)
{
    int i = 0;
    for (i=0; i<len; ++i)
        printf("%d ", arr[i]);
    printf("\n");
}

void swap(int* a, int* b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void quick_sort(int * arr, int len)
{
    if (!len)
        return;

    int p = rand() % len;
    swap(&arr[0], &arr[p]);
    //printf("%s: Before swap(len=%d, pivot=%d): ", __FUNCTION__, len, p);
    //print_array(arr, len);

    /* 2-way partition */
    int i, k = 0;
    for(i = 1; i < len; ++i) {
        if (arr[i] < arr[0])
            swap(&arr[++k], &arr[i]);
    }
    swap(&arr[0], &arr[k]);
    //printf("%s: After swap(k=%d): ", __FUNCTION__, k);
    //print_array(arr, len);

    quick_sort(&arr[0], k);
    quick_sort(&arr[k+1], len-k-1);
}

int main()
{
    int arr[] = {8,2,9,0,1,6,3};
    int len = sizeof(arr)/sizeof(int);
    srand(time(NULL));
    print_array(arr, len);
    quick_sort(arr, len);
    print_array(arr, len);
}
