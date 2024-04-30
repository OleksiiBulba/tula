#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "tula_state_machine.h"
#include "../libs/array/array.h"
// #include "libs/linked_list/linked_list.h"
#include "../libs/logger/logger.h"
#include "rule.h"

#define INITIAL_TAPE_SIZE 8
#define INITIAL_TAPE_LENGTH 0
#define INITIAL_TAPE_CHAR '0'
#define HEAD_START_POSITION 0

extern Logger *logger;
MachineState *global_machine;

/* TODO: hash table optimization
LINKED_LIST_DEFINITIONS(Rule);

DEFINE_MATCH_TYPE(
    Rule,
    CONTEXT_CRITERIA_TYPE(Rule)* c = (CONTEXT_CRITERIA_TYPE(Rule)*) context,
    (strcmp(element->state, c->state) == 0 && element->read == c->read)
)
*/

// Kinda private methods
static ARRAY(Rule)* _create_rules_array(int initial_capacity);
static bool _has_machine_halted(void);
static void _free_machine(void);
static void _print_state(void);
static void _apply_rule(const Rule*);
static void _write_tape(char);
static void _move_head(int);
static void _ensure_tape_size(int);
static void _machine_run(void);
static void _add_rule(
    char* state,
    char read,
    char write,
    int move,
    char* next_state
);
static void _set_tape_data(char*);
static bool _check_duplicate_rule(char* state, char read);

MachineState* create_machine()
{
    if (global_machine == NULL) {
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

        global_machine = machine;
    }

    return global_machine;
}

static bool _check_duplicate_rule(char* state, char read)
{
    for (int i = 0; i < global_machine->rules->count; i++) {
        if (
            strcmp(ARRAY_GET(global_machine->rules, i)->state, state) == 0
            && ARRAY_GET(global_machine->rules, i)->read == read
        ) {
            return true;
        }
    }
    return false;
}

static void _machine_run()
{
    if (global_machine->current_state == NULL || strlen(global_machine->current_state) == 0) {
        logger->fatal("No rules defined in the file.");
        exit(1);
    }

    while (1) {
        if (_has_machine_halted()) {
            _print_state();
            logger->info("The machine has halted.");
            break;
        }
        
        bool rule_found = false;
        for (int i = 0; i < global_machine->rules->count; i++) {
            if (
                strcmp(ARRAY_GET(global_machine->rules, i)->state, global_machine->current_state) == 0
                && (ARRAY_GET(global_machine->rules, i)->read == global_machine->tape[global_machine->head] || ARRAY_GET(global_machine->rules, i)->read == '_')
            ) {
                logger->debug(
                    "Found next rule: state %s, read %d, write %d, move %d.",
                    ARRAY_GET(global_machine->rules, i)->state,
                    ARRAY_GET(global_machine->rules, i)->read,
                    ARRAY_GET(global_machine->rules, i)->write,
                    ARRAY_GET(global_machine->rules, i)->move
                );

                _apply_rule(ARRAY_GET(global_machine->rules, i));
                rule_found = true;
                break;
            }
        }

        if (!rule_found) {
            logger->fatal("No rule matches the current state '%s' and read value '%c'\n", global_machine->current_state, global_machine->tape[global_machine->head]);
            exit(1);
        }
    }
}

static void _add_rule(
    char* state,
    char read,
    char write,
    int move,
    char* next_state
) {
    if (_check_duplicate_rule(state, read)) {
        logger->fatal("Ambiguous table rules for state '%s' and read value '%c'\n", state, read);
        exit(1);
    }

    if (global_machine->current_state == NULL || global_machine->rules->count == 0) {
        global_machine->current_state = strdup(state);
    }
    Rule *newRule = new_rule(state, read, write, move, next_state);
    array_append(global_machine->rules, newRule);
}

static void _set_tape_data(char* data)
{
    if (logger != NULL) {
        logger->debug("Tape data to set: \"%s\"\n", data);
    }
    if (!data) return;

    int data_len = strlen(data);
    _ensure_tape_size(data_len);
    memcpy(global_machine->tape, data, data_len);
    global_machine->tape_length = data_len;
    if (data_len < global_machine->tape_capacity) {
        global_machine->tape[data_len] = '\0';
    }
}

static void _apply_rule(const Rule *rule)
{
    _print_state();
    _write_tape(rule->write);
    _move_head(rule->move);
    #ifdef INCLUDE_PREVIOUS_STATE
    machine->previous_state = strdup(machine->current_state);
    #endif // INCLUDE_PREVIOUS_STATE
    global_machine->current_state = strdup(rule->next_state);
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

static bool _has_machine_halted()
{
    return strcmp(global_machine->current_state, HALT_STATE) == 0;
}

static void _free_machine()
{
    if (global_machine) {
        free(global_machine->current_state);
        free(global_machine->next_state);
        free(global_machine->tape);
        free(global_machine);
    }
}

static void _move_head(int steps)
{
    global_machine->head += steps;
    if (global_machine->head < 0 || global_machine->head > global_machine->tape_capacity) {
        logger->fatal("Error: Tape head out of bounds.\n");
        exit(1);
    }
}

static void _write_tape(char value)
{
    if (value != '_') {
        global_machine->tape[global_machine->head] = value;
    }
}

static void _ensure_tape_size(int new_head_position)
{
    if (new_head_position >= global_machine->tape_capacity) {
        int new_capacity = global_machine->tape_capacity * 2;
        char *new_tape = (char *)realloc(global_machine->tape, new_capacity * sizeof(global_machine->tape));
        if (new_tape == NULL) {
            logger->fatal("Error: Failed to reallocate memory for tape");
            exit(1);
        }
        memset(new_tape + global_machine->tape_capacity, INITIAL_TAPE_CHAR, new_capacity - global_machine->tape_capacity);
        global_machine->tape = new_tape;
        global_machine->tape_capacity = new_capacity;
    }
}

static void _print_state()
{
#ifdef INCLUDE_PREVIOUS_STATE
    printf("(%s -> )", machine->previous_state);
#endif
    printf("%s:", global_machine->current_state);
    for (int i = 0; i < strlen(global_machine->tape); i++) {
        printf("%c ", global_machine->tape[i]);
    }
    if (global_machine->next_state != NULL) {
        printf(" -> %s", global_machine->next_state);
    }
    printf("\n");
    int initial_offset = strlen(global_machine->current_state) + 1;
#ifdef INCLUDE_PREVIOUS_STATE
    initial_offset += 6 + strlen(machine->next_state);
#endif
    for (int i = 0; i < initial_offset + global_machine->head * 2; i++) {
        printf(" ");
    }
    printf("^\n");
}

/* TODO: hash table optimization
unsigned int hash_rule(const char* state, char read)
{
    unsigned long hash = 5381;
    int c;

    while (*state)
        hash = hash * 33 + *state++;

    return (hash * 33 + read) % HASH_TABLE_SIZE;
}

RuleHashTable* create_rule_hash_table(int size)
{
    RuleHashTable* table = malloc(sizeof(RuleHashTable));
    if (table) {
        table->entries = malloc(sizeof(RuleBucket*) * size);
        table->size = size;
        memset(table->entries, 0, sizeof(RuleBucket*) * size);
    }
    return table;
}

void add_rule_to_hash_table(RuleHashTable *table, Rule *rule)
{
    if (table && rule) {
        unsigned int index = hash_rule(rule->state, rule->read);
    }
}
*/

