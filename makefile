# src directory makefile

TARGET := yash

# Important directories
BIN_DIR := bin
CW_DIR := $(shell pwd)
INC_DIR := include
LIB_DIR := lib
OBJ_DIR := obj
SRC_DIR := src

# Define compiler and flags
CC := gcc
PFLAGS := -I$(INC_DIR)
CFLAGS := -std=c11 -Wall -Werror
#LDFLAGS := -Llib
#LDLIBS := -lm

DEP := $(wildcard $(INC_DIR)/*.h)
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

.PHONY: all clean

all: $(TARGET)

debug: CFLAGS += -g
debug: $(TARGET)

$(TARGET): $(OBJ)
	mkdir -p $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $(BIN_DIR)/$@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEP) | $(OBJ_DIR)
	$(CC) $(PFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

clean:
	$(RM) $(OBJ)
	rm -f core $(BIN_DIR)/$(TARGET)

