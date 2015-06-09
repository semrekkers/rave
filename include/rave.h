#ifndef RAVE_H_
#define RAVE_H_

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !(defined(__cplusplus) || defined(true))
typedef unsigned char bool;
#define true 1
#define false 0
#endif

#define ERR (-1)

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long long int ullong;

void rv_log(const char *file, const uint line, const char *fmt, ...);
void rv_error(const char *file, const uint line, const char *fmt, ...);
void rv_checkFailed(const char *file, const uint line, const char *cond);
#define RV_ERROR(msg) rv_error(__FILE__, __LINE__, msg)
#define RV_LOG(msg) rv_log(__FILE__, __LINE__, msg)

#ifdef DEBUG
#define RV_CHECK(cond) if (!(cond)) { rv_checkFailed(__FILE__, __LINE__, #cond); }
#else
#define RV_CHECK(cond)
#endif

// utils
void* rv_malloc(uint size);
void* rv_realloc(void *old, uint newSize);
#define rv_free(ptr) free(ptr)

char* rv_strdup(char *str);

// vector
typedef struct {
    uchar *root;
    uint isize; // item size
    uint length;
    uint cap;   // capacity
} rv_vector;

#define VECTOR_INIT_SIZE 4

int rv_vector_init(rv_vector *vec, uint size, uint itemSize);
void rv_vector_destroy(rv_vector *vec);

int rv_vector_add(rv_vector *vec, void *item);

int rv_vector_addArray(rv_vector *vec, uint arraySize, void *array);

int rv_vector_insert(rv_vector *vec, uint index, void *item);

int rv_vector_delete(rv_vector *vec, uint index);

void* rv_vector_remove(rv_vector *vec, uint index);

void* rv_vector_at(rv_vector *vec, uint index);

int rv_vector_clear(rv_vector *vec);

int rv_vector_truncate(rv_vector *vec);

int rv_vector_resize(rv_vector *vec, uint nsize);

int rv_vector_push(rv_vector *vec, void *item);

void* rv_vector_pop(rv_vector *vec);

int rv_vector_copy(rv_vector *vec, rv_vector *dest);

int rv_vector_clone(rv_vector *vec, rv_vector *dest);

void* rv_vector_toArray(rv_vector *vec);

// map

// !! WARNING !!
//
// sizes and slots fields must always be a power of two in these objects:
//  - rv_map->slots
//  - rv_map_init(uint size)
//  - rv_map_resize(uint newSize)

typedef struct rv_map_item {
    char *key;
    void *value;
    struct rv_map_item *next; // previous item in slot
    struct rv_map_item *prev; // next item in slot
} rv_map_item;

typedef struct {
    rv_map_item *first;
} rv_map_slot;

typedef struct {
    rv_map_slot *root;
    uint count; // amount of items
    uint slots; // amount of rv_map_slots in root
} rv_map;

int rv_map_init(rv_map *map, uint size);
void rv_map_destroy(rv_map *map);

rv_map_item* rv_map_at(rv_map *map, char *key);

rv_map_item* rv_map_get(rv_map *map, char *key);

int rv_map_delete(rv_map *map, rv_map_item *item);

int rv_map_deleteWithKey(rv_map *map, char *key);

bool rv_map_contains(rv_map *map, char *key);

int rv_map_clear(rv_map *map, bool init);

int rv_map_copy(rv_map *map, rv_map *dest);

int rv_map_resize(rv_map *map, uint newSize);

typedef struct {
    rv_map_item *cur;   // current item
    uint slot;          // current slot
    rv_map *map;        // map reference
} rv_mapit;

rv_mapit rv_map_iterate(rv_map *map);

rv_map_item* rv_map_next(rv_mapit *it);

#ifdef __cplusplus
}
#endif

#endif // RAVE_H_
