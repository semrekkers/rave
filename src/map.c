#include "rave.h"

#include <string.h>

static inline ullong get_slot_index(rv_map *map, char *key) {
    char *cur = key;
    ullong hash = 0xCBF29CE484222325;
    while (*cur) {
        hash ^= *(cur++);
        hash *= 0x100000001B3;
    }

    // check out the big warning in rave.h
    // because this is why we warned you
    return hash & (map->slots - 1);
}

static inline bool cmp_keys(char *keyA, char *keyB) {
    char *a = keyA;
    char *b = keyB;
    for (;;) {
        if (*(a) != *(b++))
            return false;
        if (*(a++) == '\0')
            return true;
    }
}

int rv_map_init(rv_map *map, uint size) {
    RV_CHECK(map);
    RV_CHECK(size % 2); // you did not see that warning?
    uint containerSize = sizeof(rv_map_slot) * size;
    map->root = (rv_map_slot*)rv_malloc(containerSize);
    memset(map->root, 0, containerSize);
    map->slots = size;
    map->count = 0;
    return 0;
}

void rv_map_destroy(rv_map *map) {
    rv_map_clear(map, false);
    rv_free(map->root);
}

rv_map_item* rv_map_at(rv_map *map, char *key) {
    RV_CHECK(map);
    RV_CHECK(key);
    rv_map_slot *slot = map->root + get_slot_index(map, key);
    rv_map_item *cur = slot->first;
    rv_map_item *last = cur;
    rv_map_item *item;

    if (cur) {
        while (cur) {
            if (cmp_keys(cur->key, key))
                return cur;
            last = cur;
            cur = cur->next;
        }
    }
    // key not exist, create a new item
    item = (rv_map_item*)rv_malloc(sizeof(rv_map_item));
    item->key = rv_strdup(key);
    // initialize value, set to NULL
    item->value = NULL;
    item->next = NULL;
    item->prev = last;
    last->next = item;
    map->count++;
    return item;
}

rv_map_item* rv_map_get(rv_map *map, char *key) {
    RV_CHECK(map);
    RV_CHECK(key);
    rv_map_item *cur = (map->root + get_slot_index(map, key))->first;
    while (cur) {
        if (cmp_keys(cur->key, key))
            return cur;
        cur = cur->next;
    }
    return NULL;
}

int rv_map_delete(rv_map *map, rv_map_item *item) {
    RV_CHECK(map);
    if (!item)
        return ERR;
    if (!item->prev) {
        // first item in slot, must resolve slot index for deletion
        rv_map_slot *slot = map->root + get_slot_index(map, item->key);
        slot->first = item->next;
        slot->first->prev = NULL;
    }
    else {
        item->prev->next = item->next;
        if (!item->next)
            item->next->prev = item->prev;
    }
    rv_free(item->key);
    rv_free(item);
    map->count--;
    return 0;
}

int rv_map_deleteWithKey(rv_map *map, char *key) {
    RV_CHECK(map);
    if (!key)
        return ERR;
    rv_map_item *item = rv_map_get(map, key);
    if (!item)
        return ERR;
    return rv_map_delete(map, item);
}

bool rv_map_contains(rv_map *map, char *key) {
    RV_CHECK(map);
    RV_CHECK(key);
    rv_map_item *cur = (map->root + get_slot_index(map, key))->first;
    while (cur) {
        if (cmp_keys(cur->key, key))
            return true;
        cur = cur->next;
    }
    return false;
}

int rv_map_clear(rv_map *map, bool init) {
    RV_CHECK(map);
    if (map->count) {
        rv_map_item *cur;
        rv_map_item *tmp;
        for (uint i = 0; i < map->slots; i++) {
            cur = map->root[i].first;
            while (cur) {
                tmp = cur->next;
                rv_free(cur->key);
                rv_free(cur);
                cur = tmp;
            }
        }
        map->count = 0;
        if (init)
            memset(map->root, 0, sizeof(rv_map_slot) * map->slots);
    }
    return 0;
}

int rv_map_copy(rv_map *map, rv_map *dest) {
    RV_CHECK(map);
    RV_CHECK(dest);
    rv_map_item *cur;
    for (uint i = 0; i < map->slots; i++) {
        cur = map->root[i].first;
        while (cur) {
            rv_map_at(dest, cur->key)->value = cur->value;
            cur = cur->next;
        }
    }
    return 0;
}

int rv_map_resize(rv_map *map, uint newSize) {
    RV_CHECK(map);
    RV_CHECK(newSize % 2);  // you did not see that warning?
    rv_map newMap;
    rv_map_init(&newMap, newSize);
    rv_map_copy(map, &newMap);
    rv_map_destroy(map);
    *map = newMap;
    return 0;
}

rv_mapit rv_map_iterate(rv_map *map) {
    RV_CHECK(map);
    rv_mapit it;
    it.map = map;
    it.slot = 0;
    it.cur = map->root[0].first;
    return it;
}

rv_map_item* rv_map_next(rv_mapit *it) {
    RV_CHECK(it);
    rv_map_item *ret;
    if (!it->cur) {
        ret = it->cur;
        it->cur = ret->next;
        return ret;
    }
    // search for next slot with slot->first != NULL
    for (;;) {
        it->slot++;
        if (it->slot >= it->map->slots)
            return NULL;
        it->cur = it->map->root[it->slot].first;
        if (!it->cur)
            continue;
        ret = it->cur;
        it->cur = ret->next;
        return ret;
    }
}
