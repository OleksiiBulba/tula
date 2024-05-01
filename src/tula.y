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

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../libs/array/array.h"
#include "rule.h"
#include "tula_state_machine.h"

#define LOGGER_IMPLEMENTATION
#include "../libs/logger/logger.h"

#if defined(DEBUG) + defined(RELEASE) + defined(TEST) > 1
#error "Only one of DEBUG, RELEASE, or TEST may be defined."
#endif

#if defined(DEBUG)
#define LOG_LEVEL LOG_DEBUG
#elif defined(RELEASE)
#define LOG_LEVEL LOG_ERROR
#elif defined(TEST)
#define LOG_LEVEL LOG_NONE
#else
#define LOG_LEVEL LOG_INFO
#endif

#define no_log_file NULL

extern FILE* yyin;

void yyerror(const char *s);
int yyparse(void);
int yylex(void);

MachineState *machine;
Logger *logger;

int main(int argc, char *argv[])
{ 
    logger = create_logger(LOG_LEVEL, no_log_file, LOGGER_TIME_FORMAT_ISO8601);

    if (argc < 2) {
        logger->fatal("Usage: %s <file.tula>", argv[0]);
        exit(EXIT_FAILURE);
    }

    machine = create_machine();
    if (!machine) {
       logger->fatal("Cannot create tula machine.");
       exit(EXIT_FAILURE);
    }

    yyin = fopen(argv[1], "r");
    if (!yyin) {
        logger->fatal("Failed to open tula file.");
        exit(EXIT_FAILURE);
    }
 
#if YYDEBUG
    extern int yydebug;
    yydebug = 1;
#endif

    yyparse();
    fclose(yyin);

    machine->run();
    machine->free();
    logger->free();

    return 0;
}

%}

%union {
    int num;
    char *str;
}

%token <str> TAPE
%token <str> CASE
%token <str> IDENTIFIER
%token <int> R_ARROW L_ARROW
%token R_BRAKET L_BRAKET
%token NUMBER NEWLINE

%type <num> NUMBER
%type <str> tape_content

%%

program:
    tape NEWLINE
    rules
    ;

tape:
    TAPE tape_content { machine->set_tape_data($2); }
    ;

tape_content:
      tape_content NUMBER {
        char* new_data = malloc(strlen($1) + 2);
        strcpy(new_data, $1);
        new_data[strlen($1)] = '0' + $2;
        new_data[strlen($1) + 1] = '\0';
        $$ = new_data;
        free($1);
      }
    | { $$ = strdup(""); }
    ;

rules:
    | rules rule NEWLINE { /* Nothing here */ }

rule:
    CASE IDENTIFIER NUMBER NUMBER R_ARROW IDENTIFIER    { machine->add_rule($2, '0' + $3, '0' + $4,  1, $6); }
    | CASE IDENTIFIER NUMBER NUMBER L_ARROW IDENTIFIER  { machine->add_rule($2, '0' + $3, '0' + $4, -1, $6); }
    | CASE IDENTIFIER '_' NUMBER R_ARROW IDENTIFIER     { machine->add_rule($2, '_',      '0' + $4,  1, $6); }
    | CASE IDENTIFIER NUMBER '_' R_ARROW IDENTIFIER     { machine->add_rule($2, '0' + $3, '_',       1, $6); }
    | CASE IDENTIFIER '_' '_' R_ARROW IDENTIFIER        { machine->add_rule($2, '_',      '_',       1, $6); }
    | CASE IDENTIFIER '_' NUMBER L_ARROW IDENTIFIER     { machine->add_rule($2, '_',      '0' + $4, -1, $6); }
    | CASE IDENTIFIER NUMBER '_' L_ARROW IDENTIFIER     { machine->add_rule($2, '0' + $3, '_',      -1, $6); }
    | CASE IDENTIFIER '_' '_' L_ARROW IDENTIFIER        { machine->add_rule($2, '_',      '_',      -1, $6); }
    ;

%%

void yyerror(const char *s)
{
    extern int yylineno;
    extern char *yytext;
    extern int yychar;
    logger->error("Error near %s at line %d: %s.", yytext, yylineno, s);
    if (yychar != 0) {
        logger->error("Error caused by token %d (char %c).", yychar, yychar);
    }
}

