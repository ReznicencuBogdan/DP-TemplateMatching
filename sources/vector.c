#include "vector.h"
#include <stdlib.h>
#include <string.h>

#define grow_factor 1.5
#define df_no_of_elemnts 8
#define min_no_of_elemnts 2


bool vector_realloc(vector* vector, size_t new_count) {
  const size_t new_size = new_count * vector->element_size;

  char* new_data = (char*)realloc(vector->data, new_size);
  if (!new_data) return false;

  vector->reserved_size = new_size;
  vector->data = new_data;
  return true;
}

pvector vector_create(size_t count_elements, size_t size_of_element) {
  vector* v = (vector*)malloc(sizeof(vector));

  if (v != NULL) {
    v->data = NULL;
    v->count = 0;
    v->element_size = size_of_element;

    if (count_elements < min_no_of_elemnts)
    count_elements = df_no_of_elemnts;

    if (size_of_element < 1 || !vector_realloc(v, count_elements)) {
      free(v);
      v = NULL;
    }
  }

  return v;
}

void vector_release(vector* vector) {
  if (vector->reserved_size != 0)
  free(vector->data);

  free(vector);
}

pvoid vector_at(vector* vector, size_t index) {
  return vector->data + index * vector->element_size;
}

pvoid vector_begin(vector* vector) {
  return vector->data;
}

pvoid vector_end(vector* vector) {
  return vector->data + vector->element_size * vector->count;
}

pvoid vector_next(vector* vector, pvoid i) {
  return i + vector->element_size;
}

size_t vector_count(const vector* vector) {
  return vector->count;
}

size_t vector_max_count(const vector* vector) {
  return vector->reserved_size / vector->element_size;
}

bool vector_reserve_count(vector* vector, size_t new_count) {
  if (new_count < vector->count) return false;

  size_t new_size = vector->element_size * new_count;

  if (new_size == vector->reserved_size) return true;

  return vector_realloc(vector, new_count);
}

bool vector_erase(vector* vector, size_t index) {
  if (!memmove(vector_at(vector, index),vector_at(vector, index + 1), vector->element_size * (vector->count - index)))
    return false;

  vector->count--;
  return true;
}

bool vector_append(pvector vector, const pvoid values, size_t count) {
  size_t count_new = count + vector_count(vector);

  if (vector_max_count(vector) < count_new) {
    size_t max_count_to_reserved = vector_max_count(vector) * grow_factor;

    while (count_new > max_count_to_reserved) max_count_to_reserved *= grow_factor;

    if (!vector_realloc(vector, max_count_to_reserved)) return false;
  }

  if (memcpy(vector->data + vector->count * vector->element_size, values, vector->element_size * count) == NULL)
    return false;

  vector->count = count_new;
  return true;
}

bool vector_push_back(pvector vector, const pvoid value) {
  if (!vector_append(vector, value, 1)) return false;
  return true;
}
