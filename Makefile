CC :=cc
CFLAGS := -ggdb
LEX := flex
YACC := bison
LEXFLAGS :=
YACCFLAGS := -d

SRC_DIR := src
BIN_DIR := bin
LIBS_DIR := libs

EXECUTABLE := $(BIN_DIR)/tula
LEX_SRC := $(SRC_DIR)/tula.l
YACC_SRC := $(SRC_DIR)/tula.y
LEX_OUT := $(SRC_DIR)/tula.lex.c
YACC_TAB_C := $(SRC_DIR)/tula.tab.c
YACC_TAB_H := $(SRC_DIR)/tula.tab.h

C_FILES := $(SRC_DIR)/rule.c $(SRC_DIR)/tula_state_machine.c
OBJECTS := $(YACC_TAB_C) $(LEX_OUT) $(C_FILES)

LIBS_HEADERS := $(LIBS_DIR)/array/array.h $(LIBS_DIR)/logger/logger.h

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(LIBS_HEADERS)
	$(CC) -o $@ $(OBJECTS) -lfl $(CFLAGS)

$(EXECUTABLE)-test: $(OBJECTS) $(LIBS_HEADERS)
	$(CC) -o $@ $(OBJECTS) -lfl $(CFLAGS) -DTEST

$(YACC_TAB_C): $(YACC_SRC)
	$(YACC) -o $(YACC_TAB_C) -d $<

$(LEX_OUT): $(LEX_SRC) $(YACC_TAB_H)
	$(LEX) $(LEXFLAGS) -o $@ $<

clean:
	rm -f $(EXECUTABLE) $(YACC_TAB_C) $(YACC_TAB_H) $(LEX_OUT)

