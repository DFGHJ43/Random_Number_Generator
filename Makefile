CC       = gcc
CFLAGS   = -Wall -Wextra -std=c99 -pedantic
LDFLAGS  = -lm

SRC_DIR  = src
OBJ_DIR  = obj

# ── Targets ────────────────────────────────────────────
CLI_TARGET = rng
TUI_TARGET = rng-tui

# ── Shared object files (no main) ──────────────────────
SHARED_SRCS = $(SRC_DIR)/random.c $(SRC_DIR)/stats.c $(SRC_DIR)/output.c
SHARED_OBJS = $(SHARED_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

.PHONY: all clean

all: $(CLI_TARGET) $(TUI_TARGET)

# ── CLI executable ─────────────────────────────────────
$(CLI_TARGET): $(OBJ_DIR)/main.o $(SHARED_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# ── TUI executable ─────────────────────────────────────
$(TUI_TARGET): $(OBJ_DIR)/tui.o $(SHARED_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# ── Compile rules ──────────────────────────────────────
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# ── Clean ──────────────────────────────────────────────
clean:
	rm -rf $(OBJ_DIR) $(CLI_TARGET) $(CLI_TARGET).exe $(TUI_TARGET) $(TUI_TARGET).exe
