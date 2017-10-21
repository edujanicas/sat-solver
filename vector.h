#ifndef _VECTOR_
#define _VECTOR_

#include <stdio.h>
#include <stdlib.h>

#define VECTOR_INIT_CAPACITY 100
#define VECTOR_CAPACITY_INCREASE_FACTOR 2
#define VECTOR_CAPACITY_DECREASE_FACTOR 2
#define VECTOR_CAPACITY_DECREASE_THRESHOLD 4

typedef struct vector *V;
typedef struct vector {
    void **items;
    int capacity;
    int total;
} vector;

V VECTORinit();
int VECTORtotal(V);
static void VECTORresize(V, int);
void VECTORadd(V, void *);
void VECTORset(V, int, void *);
void* VECTORget(V, int);
void VECTORdelete(V, int);
void VECTORpop(V);
void VECTORshrink(V, int n);
void VECTORfree(V);

#endif
