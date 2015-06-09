#include "rave.h"

#include <string.h>

static inline void vec_expand(rv_vector *vec) {
    if (vec->length == vec->cap)
        rv_vector_resize(vec, (vec->cap << 1));
}

static inline uchar* vec_at(rv_vector *vec, uint index) {
    return (vec->root + (vec->isize * index));
}

static inline void vec_shift(rv_vector *vec, uint from, uint to) {
    uint chunkSize = (vec->length - from) * vec->isize;
    memmove(vec_at(vec, to), vec_at(vec, from), chunkSize);
}

int rv_vector_init(rv_vector *vec, uint size, uint itemSize) {
    RV_CHECK(vec);
    vec->isize = itemSize;
    vec->root = (uchar*)rv_malloc(size * itemSize);
    vec->cap = size;
    vec->length = 0;
    return 0;
}

void rv_vector_destroy(rv_vector *vec) {
    RV_CHECK(vec);
    rv_free(vec->root);
    vec->root = NULL;
}

int rv_vector_add(rv_vector *vec, void *item) {
    RV_CHECK(vec);
    vec_expand(vec);
    memcpy(vec_at(vec, vec->length), item, vec->isize);
    vec->length++;
    return 0;
}

int rv_vector_addArray(rv_vector *vec, uint arraySize, void *array) {
    RV_CHECK(vec);
    RV_CHECK(array);
    // expand if needed
    if ((vec->length + arraySize) >= vec->cap)
        rv_vector_resize(vec, (vec->length + arraySize));
    memcpy(vec_at(vec, vec->length), array, (vec->isize * arraySize));
    vec->length += arraySize;
    return 0;
}

int rv_vector_insert(rv_vector *vec, uint index, void *item) {
    RV_CHECK(vec);
    // check if index is out of range
    if (index > vec->length)
        return ERR;
    vec_expand(vec);
    if (index != vec->length) {
        // shift all items before index one place up
        vec_shift(vec, index, index + 1);
    }
    memcpy(vec_at(vec, vec->length), item, vec->isize);
    vec->length++;
    return 0;
}

int rv_vector_delete(rv_vector *vec, uint index) {
    RV_CHECK(vec);
    // check if index is out of range
    if (index >= vec->length)
        return ERR;
    vec_shift(vec, (index + 1), index);
    vec->length--;
    return 0;
}

void* rv_vector_remove(rv_vector *vec, uint index) {
    RV_CHECK(vec);
    // check if index is out of range
    if (index >= vec->length)
        return NULL;
    // copy item from vector
    void *item = rv_malloc(vec->isize);
    memcpy(item, vec_at(vec, index), vec->isize);
    // delete (shift) item from vector
    vec_shift(vec, (index + 1), index);
    vec->length--;
    return item;
}

void* rv_vector_at(rv_vector *vec, uint index) {
    RV_CHECK(vec);
    return vec_at(vec, index);
}

int rv_vector_clear(rv_vector *vec) {
    RV_CHECK(vec);
    rv_free(vec->root);
    vec->root = (uchar*)rv_malloc(VECTOR_INIT_SIZE * vec->isize);
    vec->cap = VECTOR_INIT_SIZE;
    vec->length = 0;
    return 0;
}

int rv_vector_truncate(rv_vector *vec) {
    RV_CHECK(vec);
    vec->length = 0;
    return 0;
}

int rv_vector_resize(rv_vector *vec, uint nsize) {
    RV_CHECK(vec);
    vec->root = rv_realloc(vec->root, (vec->isize * nsize));
    vec->cap = nsize;
    return 0;
}

int rv_vector_push(rv_vector *vec, void *item) {
    return rv_vector_add(vec, item);
}

void* rv_vector_pop(rv_vector *vec) {
    return rv_vector_remove(vec, vec->length - 1);
}

int rv_vector_clone(rv_vector *vec, rv_vector *dest) {
    RV_CHECK(vec);
    RV_CHECK(dest);
    dest->isize = vec->isize;
    dest->cap = vec->cap;
    dest->length = vec->length;
    // copy items from vec to dest
    uint vecSize = vec->isize * vec->cap;
    dest->root = (uchar*)rv_malloc(vecSize);
    memcpy(dest->root, vec->root, vecSize);
    return 0;
}

void* rv_vector_toArray(rv_vector *vec) {
    RV_CHECK(vec);
    return vec->root;
}
