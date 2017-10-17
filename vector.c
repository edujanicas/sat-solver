#include "vector.h"

void VECTORinit(V v)
{
    v->capacity = VECTOR_INIT_CAPACITY;
    v->total = 0;
    v->items = malloc(sizeof(void *) * v->capacity);
}

int VECTORtotal(V v)
{
    return v->total;
}

static void VECTORresize(V v, int capacity)
{
    void **items = realloc(v->items, sizeof(void *) * capacity);
    if (items) {
        v->items = items;
        v->capacity = capacity;
    }
}

void VECTORadd(V v, void *item)
{
    if (v->capacity == v->total)
        VECTORresize(v, v->capacity * 2);
    v->items[v->total++] = item;
}

void VECTORset(V v, int index, void *item)
{
    if (index >= 0 && index < v->total)
        v->items[index] = item;
}

void *VECTORget(V v, int index)
{
    if (index >= 0 && index < v->total)
        return v->items[index];
    return NULL;
}

void VECTORdelete(V v, int index)
{
    if (index < 0 || index >= v->total)
        return;

    v->items[index] = NULL;

    for (int i = index; i < v->total - 1; i++) {
        v->items[i] = v->items[i + 1];
        v->items[i + 1] = NULL;
    }

    v->total--;

    if (v->total > 0 && v->total == v->capacity / 4)
        VECTORresize(v, v->capacity / 2);
}

void VECTORpop(V v)
{
    VECTORdelete(v, v->total - 1);
}

void VECTORshrink(V v, int n)
{
    for(int i = 0; i < n; i++) {
      VECTORdelete(v, v->total - 1);
    }
}

void VECTORfree(V v)
{
    free(v->items);
}
