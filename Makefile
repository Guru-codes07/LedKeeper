# Makefile - LedKeeper
#
# Build:
#   make            # compiles src/*.c -> ./ledkeeper
# Install (user-local, no sudo required):
#   make install    # binary -> ~/.local/bin, unit -> ~/.config/systemd/user
# Remove:
#   make uninstall
# Clean build artifacts:
#   make clean

CC      := gcc
CFLAGS  := -Wall -Wextra -O2

SRC_DIR := src
SRCS    := $(wildcard $(SRC_DIR)/*.c)
OBJS    := $(SRCS:.c=.o)
BIN     := ledkeeper

# XDG-friendly install locations. Override on the command line if
# needed, e.g. `make install PREFIX=/usr/local`.
PREFIX           := $(HOME)/.local
BINDIR           := $(PREFIX)/bin
SYSTEMD_USER_DIR := $(HOME)/.config/systemd/user

.PHONY: all clean install uninstall

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c $(SRC_DIR)/%.h
	$(CC) $(CFLAGS) -c $< -o $@

# main.o has no matching main.h, so it needs its own rule (the
# pattern rule above requires a same-named .h to exist).
$(SRC_DIR)/main.o: $(SRC_DIR)/main.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SRC_DIR)/*.o $(BIN)

install: $(BIN)
	install -d $(BINDIR) $(SYSTEMD_USER_DIR)
	install -m 755 $(BIN) $(BINDIR)/$(BIN)
	install -m 644 systemd/ledkeeper.service $(SYSTEMD_USER_DIR)/ledkeeper.service
	@echo "Installed $(BIN) to $(BINDIR)"
	@echo "Installed systemd unit to $(SYSTEMD_USER_DIR)"
	@echo ""
	@echo "Next steps:"
	@echo "  systemctl --user daemon-reload"
	@echo "  systemctl --user enable --now ledkeeper.service"

uninstall:
	systemctl --user disable --now ledkeeper.service 2>/dev/null || true
	rm -f $(BINDIR)/$(BIN)
	rm -f $(SYSTEMD_USER_DIR)/ledkeeper.service
	systemctl --user daemon-reload
	@echo "Uninstalled $(BIN) and its systemd unit."
