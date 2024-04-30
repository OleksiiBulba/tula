CC :=cc
CFLAGS := -ggdb
LEX := flex
YACC := bison
LEXFLAGS :=
YACCFLAGS := -d

SRC_DIR := src
BIN_DIR := bin

EXECUTABLE := $(BIN_DIR)/tula
LEX_SRC := $(SRC_DIR)/tula.l
YACC_SRC := $(SRC_DIR)/tula.y
LEX_OUT := $(SRC_DIR)/tula.lex.c
YACC_TAB_C := $(SRC_DIR)/tula.tab.c
YACC_TAB_H := $(SRC_DIR)/tula.tab.h

OTHER_SOURCES := $(SRC_DIR)/rule.c $(SRC_DIR)/tula_state_machine.c
OBJECTS := $(YACC_TAB_C) $(LEX_OUT) $(OTHER_SOURCES)

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(YACC_TAB_C) $(LEX_OUT) $(OTHER_SOURCES)
	$(CC) -o $@ $^ -lfl $(CFLAGS)

$(YACC_TAB_C): $(YACC_SRC)
	$(YACC) -o $(YACC_TAB_C) -d $<

$(LEX_OUT): $(LEX_SRC) $(YACC_TAB_H)
	$(LEX) $(LEXFLAGS) -o $@ $<

clean:
	rm -f $(EXECUTABLE) $(YACC_TAB_C) $(YACC_TAB_H) $(LEX_OUT)

