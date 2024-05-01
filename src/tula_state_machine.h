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

typedef struct MachineState {
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

    void (*run)(struct MachineState *machine);
    void (*add_rule)(struct MachineState *machine, char* state, char read, char write, int move, char* next_state);
    void (*set_tape_data)(struct MachineState *machine, char* data);
    void (*free)(struct MachineState *machine);
} MachineState;

extern MachineState *global_machine;

MachineState* create_machine();

typedef void (*tape_reader)(FILE *);

#endif // STATE_MACHINE_H_
