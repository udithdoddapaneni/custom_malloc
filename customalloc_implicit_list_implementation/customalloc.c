#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int getsize(int* curr);
int is_alloc(int * curr);
int* ni(int *curr);

int* cusalloc(int size, int** start, int** last){
    if (*last == *start){
        brk(*start);
        sbrk(size*sizeof(int));
        *(*start-1) = (size << 1)^1;
        *(*start+size) = (size << 1)^1;
        *last = *start+size+2; // should point to null: above heap and in unallocated space
        return *start; // also curr
    }
    
    int* curr = *start;
    while (curr < *last && (getsize(curr) < size || is_alloc(curr)))
    {
        curr = ni(curr);
    }
    
    if (curr == *last){
        sbrk((size+2)*sizeof(int));
        *(curr-1) = (size << 1)^1;
        *(curr+size) = (size << 1)^1;
        *last = curr+size+2;
        return curr;
    }
    *(curr-1) = (size << 1)^1;
    *(curr+size) = (size << 1)^1;
    return curr;
}

int is_alloc(int * curr){
    return *(curr-1) & 1;
}


int getsize(int* curr){
    return *(curr-1)/2;
}

int* ni(int *curr){
    return curr+getsize(curr)+2;
}

int* pi(int *curr, int **start){
    if (curr == *start){
        printf("no previous block\n");
        exit(-1);
    }
    return curr - 2 - (*(curr - 2)/2);
}

int initialization(int** start, int** last){
    sbrk(3*sizeof(int));
    *start = sbrk(0)+1;
    *(*start+1) = (1 << 1)^1;
    *(*start-1) = (1 << 1)^1;
    *last = *start;
}

void freelist(int* arr, int** start, int** last){
    if (!is_alloc(arr)){
        printf("error: already allocated\n");
        exit(-1);
    }

    int* next = ni(arr);
    int* prev;
    if (arr != *start) prev = pi(arr, start);

    if (next == *last){
        *(arr-1) ^= 1;
        *last = arr;
        brk(last+1);
        return;
    }
    else if (arr == *start){
        *(arr-1) ^= 1;
        if (is_alloc(next)){
            return;
        }
        else{
            *(arr-1) += *(next-1);
            return;
        }
    }
    else{
        *(arr-1) ^= 1;
        if (is_alloc(next) && is_alloc(prev)){
            return;
        }
        else if (is_alloc(next)){
            *(prev-1) += *(next-1);
            return;
        }
        else if (is_alloc(prev)){
            *(arr-1) += *(next-1);
            return;
        }
        else{
            *(prev-1) += *(arr-1) + *(next-1);
            return;
        }
    }
}


int main()
{
    // initialization
    int* start;
    int* last;
    initialization(&start, &last);
    // initialization



    // below is all just testing
    int* first = cusalloc(4, &start, &last);
    
    first[0] = 1;
    first[1] = 2;
    first[2] = 3;
    first[3] = 4;

    int* second = cusalloc(3, &start, &last);

    second[0] = 5;
    second[1] = 6;
    second[2] = 7;

    int *third = cusalloc(2, &start, &last);

    third[0] = 8;
    third[1] = 9;

    // for (int i = 0; i < getsize(first); i++) printf("%d\n", first[i]);

    // for (int i = 0; i < getsize(second); i++) printf("%d\n", second[i]);

    // for (int i = 0; i < getsize(third); i++) printf("%d\n", third[i]);
    printf("%p\n", second);
    freelist(second, &start, &last);

    int *fourth = cusalloc(3, &start, &last);
    
    fourth[0] = 69; fourth[1] = 420; fourth[2] = 96;

    printf("%p\n", fourth);

    

    return 0;
}