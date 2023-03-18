#include "map.h"

unsigned int hash(void *key) {
    return *(unsigned int *)key;
}

void createMap(mapPtr m, int capacity) {
    m->keys = malloc(capacity * sizeof(void *));
    m->values = malloc(capacity * sizeof(void *));
    m->capacity = capacity;
    m->size = 0;
}

void insert(mapPtr m, void *key, void *value) {
    // get the hash code for the key
    int h = hash(key) % m->capacity;

    // check if the key already exists
    for (int i = 0; i < m->size; i++) {
        if (m->keys[i] != NULL && hash(m->keys[i]) == h && memcmp(m->keys[i], key, sizeof(void *)) == 0) {
            m->values[i] = value;
            return;
        }
    }

    // if the key doesn't exist, add it to the map
    while (m->keys[h] != NULL) {
        h = (h + 1) % m->capacity;
    }
    m->keys[h] = key;
    m->values[h] = value;
    m->size++;

    // if the map is getting too full, resize it
    if (m->size >= m->capacity * 0.75) {
        int new_capacity = m->capacity * 2;
        void **new_keys = malloc(new_capacity * sizeof(void *));
        void **new_values = malloc(new_capacity * sizeof(void *));
        for (int i = 0; i < m->capacity; i++) {
            if (m->keys[i] != NULL) {
                int h = hash(m->keys[i]) % new_capacity;
                while (new_keys[h] != NULL) {
                    h = (h + 1) % new_capacity;
                }
                new_keys[h] = m->keys[i];
                new_values[h] = m->values[i];
            }
        }
        free(m->keys);
        free(m->values);
        m->keys = new_keys;
        m->values = new_values;
        m->capacity = new_capacity;
    }
}

void *get(mapPtr m, void *key) {
    int h = hash(key) % m->capacity;
    while (m->keys[h] != NULL) {
        if (hash(m->keys[h]) == h && memcmp(m->keys[h], key, sizeof(void *)) == 0) {
            return m->values[h];
        }
        h = (h + 1) % m->capacity;
    }
    return NULL;
}

void remove(mapPtr m, void *key) {
    // get the hash code for the key
    int h = hash(key) % m->capacity;

    // search for the key in the map
    while (m->keys[h] != NULL) {
        if (hash(m->keys[h]) == h && memcmp(m->keys[h], key, sizeof(void *)) == 0) {
            // remove the key-value pair
            m->keys[h] = NULL;
            m->values[h] = NULL;
            m->size--;
            return;
        }
        h = (h + 1) % m->capacity;
    }
}