/*
 * MIT License
 *
 * Copyright (c) 2024 Alexey Kutepov <reximkut@gmail.com>
 * Copyright (c) 2024 Oleksii Bulba <oleksii.bulba@gmail.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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
