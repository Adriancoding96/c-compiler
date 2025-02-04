#ifndef VECTOR_H
#define VECTOR_H

// Defines initial starting size of vector
#define VECTOR_INIT_CAPACITY 20

// Defines vector macros
#define VECTOR_INIT(vec) vector vec; vector_init(&vec)
#define VECTOR_ADD(vec, item) vector_add(&vec, (void *) item)
#define VECTOR_SET(vec, id, item) vector_set(&vec, id, (void *) item)
#define VECTOR_GET(vec, type, id) (type) vector_get(&vec, id)
#define VECTOR_DELETE(vec, id) vector_delete(&vec, id)
#define VECTOR_TOTAL(vec) vector_total(&vec)
#define VECTOR_FREE(vec) vector_free(&vec)

// Vector struct
typedef struct vector {
    void **items; // Array of pointers to vector content
    int capacity; // Vector size
    int total; // Amount of items stored in vector
} vector;

// Function definitions
void vector_init(vector *); // Initiallized vector
int vector_total(vector *); // Return amount of items in vector
static void vector_resize(vector *, int); // Function to resize vector
void vector_add(vector *, void *); // Function to add a new item in the vector
void vector_set(vector *, int, void *); // Replace existing item in vector
void *vector_get(vector *, int); // Returns a item from the vector as a void pointer
void vector_delete(vector *, int); // Delete item from vector
void vector_free(vector *); // Frees memory allocated for vector

#endif
