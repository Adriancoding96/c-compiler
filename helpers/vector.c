#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

# include "vector.h"

/*
 * Initiates vector with capacity, total amout of items
 * as 0, and allocates memory on the heap for the vector.
 * */
void vector_init(vector *vec) {
    vec->capacity = VECTOR_INIT_CAPACITY;
    vec->total = 0;
    vec->items = malloc(sizeof(void *) * vec->capacity);
}


/*
 * Gets the total amount of items in the vector.
 * */
int vector_total(vector *vec) {
    return vec->total;
}


/*
 * Realocates vector on with larger heap size.
 * */
static void vector_resize(vector *vec, int capacity) {
    void **items = realloc(vec->items, sizeof(void *) * capacity);
    if(items) {
        vec->items = items;
        vec->capacity = capacity;
    }
}

/*
 * Adds a new item in to vector, if size is exceeded calls
 * vector_resize doubling the heap allocation.
 * */
void vector_add(vector *vec, void *item) {
    if(vec->capacity == vec->total) {
        vector_resize(vec, vec->capacity * 2);
    }
    vec->items[vec->total++] = item;
}

/*
 * Replaces item in the vector by specified index.
 * */
void vector_set(vector *vec, int index, void *item) {
    if(index >= 0 && index < vec->total) {
        vec->items[index] = item;
    }
}


/*
 * Gets item from vector by specified index.
 * Return a void * meaining that the item can be of any type.
 * */
void *vector_get(vector *vec, int index) {
    if(index >= 0 && index < vec->total) {
        return vec->items[index];
    }
    return NULL;
}

/*
 * Deletes item in vector.
 * Shifts items after deleted item one index backwards.
 * Checks if vector contains total amount of items equaling a quarter of the capacity,
 * if true realocates vector on the heap by half the size.
 * */
void vector_delete(vector *vec, int index) {
    if(index < 0 || index >= vec->total) {
        return;
    }

    vec->items[index] = NULL;

    for(int i = 0, n = vec->total - 1; i < n; i++) {
        vec->items[i] = vec->items[i + 1];
        vec->items[i + 1] = NULL;
    }

    vec->total--;

    if(vec->total > 0 && vec->total == vec->capacity / 4) {
        vector_resize(vec, vec->capacity / 2);
    }
}

/*
 * Frees memory allcated for vector.
 * */
void vector_free(vector *vec) {
    free(vec->items);
}
