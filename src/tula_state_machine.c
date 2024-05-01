#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "tula_state_machine.h"
#include "../libs/array/array.h"
#include "../libs/logger/logger.h"
#include "rule.h"

#define INITIAL_TAPE_SIZE 8
#define INITIAL_TAPE_LENGTH 0
#define INITIAL_TAPE_CHAR '0'
#define HEAD_START_POSITION 0

extern Logger *logger;

// Kinda private methods
static ARRAY(Rule)* _create_rules_array(int initial_capacity);
static bool _has_machine_halted(MachineState *machine);
static void _free_machine(MachineState *machine);
static void _print_state(MachineState *machine);
static void _apply_rule(MachineState *machine, const Rule*);
static void _write_tape(MachineState *machine, char);
static void _move_head(MachineState *machine, int);
static void _ensure_tape_size(MachineState *machine, int);
static void _machine_run(MachineState *machine);
static void _add_rule(
    MachineState *machine, 
    char* state,
    char read,
    char write,
    int move,
    char* next_state
);
static void _set_tape_data(MachineState *machine, char*);
static bool _check_duplicate_rule(MachineState *machine, char* state, char read);

MachineState* create_machine()
{
    MachineState* machine = malloc(sizeof(MachineState));
    if (!machine) {
        logger->fatal("Failed to allocate memory for the machine.");
        exit(EXIT_FAILURE);
    }

    machine->tape_capacity = INITIAL_TAPE_SIZE;
    machine->tape_length = INITIAL_TAPE_LENGTH;
    machine->tape = malloc(machine->tape_capacity * sizeof(char));
    if (!machine->tape) {
        free(machine);
        logger->fatal("Failed to allocate memory for the tape.");
        exit(EXIT_FAILURE);
    }

    machine->rules = _create_rules_array(INIAL_RULES_CAPACITY);
    if (!machine->rules) {
        free(machine->tape);
        free(machine);
        logger->fatal("Failed to allocate memory for rules array.");
        exit(EXIT_FAILURE);
    }

    memset(machine->tape, INITIAL_TAPE_CHAR, INITIAL_TAPE_SIZE);

    machine->current_state = "";
#ifdef INCLUDE_PREVIOUS_STATE
    machine->previous_state = "";
#endif
    machine->next_state = NULL;
    machine->head = HEAD_START_POSITION;

    machine->run = _machine_run;
    machine->add_rule = _add_rule;
    machine->free = _free_machine;
    machine->set_tape_data = _set_tape_data;

    return machine;
}

static bool _check_duplicate_rule(MachineState *machine, char* state, char read)
{
    for (int i = 0; i < machine->rules->count; i++) {
        if (
            strcmp(ARRAY_GET(machine->rules, i)->state, state) == 0
            && ARRAY_GET(machine->rules, i)->read == read
        ) {
            return true;
        }
    }
    return false;
}

static void _machine_run(MachineState *machine)
{
    if (machine->current_state == NULL || strlen(machine->current_state) == 0) {
        logger->fatal("No rules defined in the file.");
        exit(1);
    }

    while (1) {
        if (_has_machine_halted(machine)) {
            _print_state(machine);
            logger->info("The machine has halted.");
            break;
        }
        
        bool rule_found = false;
        for (int i = 0; i < machine->rules->count; i++) {
            if (
                strcmp(ARRAY_GET(machine->rules, i)->state, machine->current_state) == 0
                && (ARRAY_GET(machine->rules, i)->read == machine->tape[machine->head] || ARRAY_GET(machine->rules, i)->read == '_')
            ) {
                logger->debug(
                    "Found next rule: state %s, read %d, write %d, move %d.",
                    ARRAY_GET(machine->rules, i)->state,
                    ARRAY_GET(machine->rules, i)->read,
                    ARRAY_GET(machine->rules, i)->write,
                    ARRAY_GET(machine->rules, i)->move
                );

                _apply_rule(machine, ARRAY_GET(machine->rules, i));
                rule_found = true;
                break;
            }
        }

        if (!rule_found) {
            logger->fatal("No rule matches the current state '%s' and read value '%c'\n", machine->current_state, machine->tape[machine->head]);
            exit(1);
        }
    }
}

static void _add_rule(
    MachineState *machine, 
    char* state,
    char read,
    char write,
    int move,
    char* next_state
) {
    if (_check_duplicate_rule(machine, state, read)) {
        logger->fatal("Ambiguous table rules for state '%s' and read value '%c'\n", state, read);
        exit(1);
    }

    if (machine->current_state == NULL || machine->rules->count == 0) {
        machine->current_state = strdup(state);
    }
    Rule *newRule = new_rule(state, read, write, move, next_state);
    array_append(machine->rules, newRule);
}

static void _set_tape_data(MachineState *machine, char* data)
{
    if (logger != NULL) {
        logger->debug("Tape data to set: \"%s\"\n", data);
    }
    if (!data) return;

    int data_len = strlen(data);
    _ensure_tape_size(machine, data_len);
    memcpy(machine->tape, data, data_len);
    machine->tape_length = data_len;
    if (data_len < machine->tape_capacity) {
        machine->tape[data_len] = '\0';
    }
}

static void _apply_rule(MachineState *machine, const Rule *rule)
{
    _print_state(machine);
    _write_tape(machine, rule->write);
    _move_head(machine, rule->move);
    #ifdef INCLUDE_PREVIOUS_STATE
    machine->previous_state = strdup(machine->current_state);
    #endif // INCLUDE_PREVIOUS_STATE
    machine->current_state = strdup(rule->next_state);
}

static ARRAY(Rule)* _create_rules_array(int initial_capacity)
{
    ARRAY(Rule) *rules = malloc(sizeof(ARRAY(Rule)));
    if (rules == NULL) {
        logger->fatal("ERROR: Cannot allocate memory for array of rules.\n");
        exit(1);
    }

    rules->count = 0;
    rules->capacity = initial_capacity;
    rules->items = malloc(rules->capacity * sizeof(Rule));
    if (rules->items == NULL) {
        free(rules);
        rules = NULL;
        logger->fatal("ERROR: Cannot allocate memory for array of rules.\n");
        exit(1);
    }
    
    return rules;
}

static bool _has_machine_halted(MachineState *machine)
{
    return strcmp(machine->current_state, HALT_STATE) == 0;
}

static void _free_machine(MachineState *machine)
{
    free(machine->current_state);
    free(machine->next_state);
    free(machine->tape);
    free(machine);
}

static void _move_head(MachineState *machine, int steps)
{
    machine->head += steps;
    if (machine->head < 0 || machine->head > machine->tape_capacity) {
        logger->fatal("Error: Tape head out of bounds.\n");
        exit(1);
    }
}

static void _write_tape(MachineState *machine, char value)
{
    if (value != '_') {
        machine->tape[machine->head] = value;
    }
}

static void _ensure_tape_size(MachineState *machine, int new_head_position)
{
    if (new_head_position >= machine->tape_capacity) {
        int new_capacity = machine->tape_capacity * 2;
        char *new_tape = (char *)realloc(machine->tape, new_capacity * sizeof(machine->tape));
        if (new_tape == NULL) {
            logger->fatal("Error: Failed to reallocate memory for tape");
            exit(1);
        }
        memset(new_tape + machine->tape_capacity, INITIAL_TAPE_CHAR, new_capacity - machine->tape_capacity);
        machine->tape = new_tape;
        machine->tape_capacity = new_capacity;
    }
}

static void _print_state(MachineState *machine)
{
#ifdef INCLUDE_PREVIOUS_STATE
    printf("(%s -> )", machine->previous_state);
#endif
    printf("%s:", machine->current_state);
    for (int i = 0; i < strlen(machine->tape); i++) {
        printf("%c ", machine->tape[i]);
    }
    if (machine->next_state != NULL) {
        printf(" -> %s", machine->next_state);
    }
    printf("\n");
    int initial_offset = strlen(machine->current_state) + 1;
#ifdef INCLUDE_PREVIOUS_STATE
    initial_offset += 6 + strlen(machine->next_state);
#endif
    for (int i = 0; i < initial_offset + machine->head * 2; i++) {
        printf(" ");
    }
    printf("^\n");
}

