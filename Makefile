CC := clang

CFLAGS := -g -Wall -Werror -I include/

SRC_DIR := src
OBJ_DIR := obj

SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
EXE := shell

.PHONY := all clean

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

clean:
	@rm -rv $(OBJ_DIR) $(EXE)

-include $(OBJ:.o=.d)
