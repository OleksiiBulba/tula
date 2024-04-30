#include <stdlib.h>
#include <string.h>

#include "rule.h"

Rule* new_rule(char* state, char read, char write, int move, char* next_state)
{
    Rule* newRule = malloc(sizeof(Rule));
    
    newRule->state = strdup(state);
    newRule->read = read;
    newRule->write = write;
    newRule->move = move;
    newRule->next_state = strdup(next_state);

    return newRule;    
}

