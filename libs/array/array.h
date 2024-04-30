#ifndef ARRAY_H_
#define ARRAY_H_

#ifndef ARRAY_ASSERT
#include <assert.h>
#define ARRAY_ASSERT assert
#endif // ARRAY_ASSERT

#ifndef ARRAY_REALLOC
#include <stdlib.h>
#define ARRAY_REALLOC realloc
#endif // ARRAY_REALLOC

#ifndef ARRAY_FREE
#define ARRAY_FREE free
#endif // ARRAY_FREE

#define ARRAY_LEN(array) ((array)->count)

#define ARRAY_GET(array, index)    \
    (ARRAY_ASSERT(index >= 0), ARRAY_ASSERT(index < (array)->count), (array)->items[index])

#define ARRAY(item_type) array_##item_type

#define ARRAY_DEFINE(item_type)     \
    typedef struct {                \
        const item_type **items;    \
        size_t count;               \
        size_t capacity;            \
    } ARRAY(item_type)

#define ARRAY_INIT_CAP 8

#define array_append(array, item)                                                                           \
    do {                                                                                                    \
        if ((array)->count + 1 >= (array)->capacity) {                                                      \
            (array)->capacity = (array)->capacity == 0 ? ARRAY_INIT_CAP : (array)->capacity * 2;            \
            (array)->items = ARRAY_REALLOC((array)->items, (array)->capacity * sizeof(*((array)->items)));  \
            ARRAY_ASSERT((array)->items != NULL && (array)->capacity > 0 && "Cannot reallocate memory for a dynamic array");         \
        }                                                                                                   \
                                                                                                            \
        (array)->items[(array)->count++] = (item);                                                          \
    } while(0)


#define array_free_array(array) ARRAY_FREE((array).items)

#endif // ARRAY_H_
