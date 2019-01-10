#ifndef VECTOR_H
#define VECTOR_H

#include <stdbool.h>
#include <stdio.h>

#include "datatypes.h"

typedef struct {
  size_t count;
  size_t element_size;
  size_t reserved_size;
  char* data;
} vector;


#define pvector vector*

pvector vector_create(size_t count_elements, size_t size_of_element);

/*
* @vector_release releases the vector. */
void vector_release(vector* vector);

/*
* @vector_at returns the item at index position in the vector.*/
pvoid vector_at(vector* vector, size_t index);

/*
* @vector_begin Returns a pointer to the first item in the vector. */
pvoid vector_begin(vector* vector);

/*
* @vector_end returns a pointer to the imaginary item after the last item in the vector.*/
pvoid vector_end(vector* vector);

/*
* @vector_next returns a pointer to the next element of vector after 'i'.*/
pvoid vector_next(vector* vector, pvoid i);

/*
* @vector_count Returns the number of elements in the vector.*/
size_t vector_count(const vector* vector);

/*
* @vector_max_count returns the maximum number of elements that the vector can hold.*/
size_t vector_max_count(const vector* vector);

/*
* @vector_reserve_count Resizes the container so that it contains n elements.*/
bool vector_reserve_count(vector* vector, size_t new_count);

/*
* @vector_erase removes from the vector a single element by 'index'*/
bool vector_erase(vector* vector, size_t index);

/*
* @vector_append inserts multiple values at the end of the vector.*/
bool vector_append(vector* vector, const pvoid values, size_t count);

/*
* @vector_push_back inserts value at the end of the vector.*/
bool vector_push_back(vector* vector, const pvoid value);

#endif
