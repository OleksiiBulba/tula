#ifndef RULE_H_
#define RULE_H_

typedef struct {
    char* state;
    char read;
    char write;
    int move;
    char* next_state;
} Rule;

Rule* new_rule(char* state, char read, char write, int move, char* next_state);

#endif // RULE_H_
