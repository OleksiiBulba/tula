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

#ifndef STATE_MACHINE_H_
#define STATE_MACHINE_H_

#include <stdio.h>
#include <stdbool.h>

#include "../libs/array/array.h"
#include "rule.h"

#define TAPE_SIZE 1024
#define INIAL_RULES_CAPACITY 8
#define HALT_STATE "Halt"
#define HASH_TABLE_SIZE 137

ARRAY_DEFINE(Rule);

typedef struct {
    char* current_state;
    char* next_state;
#ifdef INCLUDE_PREVIOUS_STATE
    char* previous_state; // TODO: do we need the previous state?
#endif // INCLUDE_PREVIOUS_STATE
    char* tape;
    int tape_length;
    int tape_capacity;
    int head;
    ARRAY(Rule) *rules;

    void (*run)();
    void (*add_rule)(char* state, char read, char write, int move, char* next_state);
    void (*read_tape)(FILE *file);
    void (*add_tape_data)(char data);
    void (*set_tape_data)(char* data);
    void (*free)();
} MachineState;

extern MachineState *global_machine;

MachineState* create_machine();

typedef void (*tape_reader)(FILE *);

#endif // STATE_MACHINE_H_
